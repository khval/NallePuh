/*
	Copyright (C) 2023: Kjetil Hvalstrand
	MIT License.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

#include <dos/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/emulation.h>
#include <exec/interrupts.h>
#include <devices/ahi.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include <proto/exec.h>

#include "PUH.h"
#include "emu_cia.h"

extern struct TagItem SchedulerState_tags[];
extern void debug_info( struct ExceptionContext *pContext, struct ExecBase *pSysBase, APTR pFaultAddress );

static uint32 VSYNC_COUNTER = 0;
static uint32 VSYNC_COUNTER_LATCH = 0;

struct chip chip_ciaa ;

#define SET_CRA(bit,set) SET_REG_BIT(chip_ciaa.a.cr,bit,set)
#define SET_CRB(bit,set) SET_REG_BIT(chip_ciaa.a.cr,bit,set)

static uint32 TIMER_A_us = 0;
static uint32 TIMER_B_us = 0;

#define TIMER_A			chip_ciaa.a.ticks
#define TIMER_A_LATCH		chip_ciaa.a.ticks_latch
#define TIMER_A_ticks		chip_ciaa.a.new_ticks

#define TIMER_B			chip_ciaa.b.ticks
#define TIMER_B_LATCH		chip_ciaa.b.ticks_latch
#define TIMER_B_ticks		chip_ciaa.b.new_ticks

static uint32 TIMER_V_us = 0;
static uint32 TIMER_V_ticks = 0;

static bool te_start_set = false;
struct timeval cia_te_start ;

extern struct Process *MainTask;
extern uint32 SchedulerState;

ULONG ciaa_pra = 0xC0;	// bit 7,6 has pull up's

static bool cia_get_usec( int *out_usec )
{
	if (! SchedulerState)		// not in forbid state
	{
		struct timeval cia_te ;
		struct timeval cia_te_diff ;
		int usec;

		if (te_start_set)
		{
			gettimeofday(&cia_te, NULL);
			timersub(&cia_te,&cia_te_start,&cia_te_diff);
			cia_te_start = cia_te;

			usec = (cia_te_diff.tv_sec * 1000000) + cia_te_diff.tv_usec ;
			*out_usec = usec > 0 ? usec : 1;		// only posetive values, no overflow values allowed.
		}

		gettimeofday(&cia_te_start, NULL);
		te_start_set = true;

		return true;
	}
	else
	{
		return false;		
	}
}



void update_timer_ciaa()
{
	int us = 0;
	bool state_update = false;

	// calulate delta time... from last read..

	if ( cia_get_usec( &us ) == false )
	{
		Signal(MainTask, chip_ciaa.signal );
		return;
	}

	// time us accumulators, us is subtracted when number is dividable.

	TIMER_A_us += us;
	TIMER_B_us += us;
	TIMER_V_us += us;

	// convert delta time in us to ticks
	
	if ( (TIMER_A) && us2ticks( CIA_TIMER_UNIT, &TIMER_A_us , &TIMER_A_ticks ) )  	// count down mode, single shot... :-P
	{
		do_cia_timer_a( &chip_ciaa);
		state_update = true;
	}

	if ( (TIMER_B) && us2ticks( CIA_TIMER_UNIT, &TIMER_B_us, &TIMER_B_ticks ) )	        // count down mode, single shot... :-P
	{
		do_cia_timer_b( &chip_ciaa);
		state_update = true;
	}

	// PAL mode 50HZ 

	if ( us2ticks( VSYNC_HZ, &TIMER_V_us, &TIMER_V_ticks ) )	        // count down mode, single shot... :-P
	{
		VSYNC_COUNTER = (VSYNC_COUNTER + TIMER_V_ticks) & 0xFFFFFF;
		state_update = true;
	}
/*
	if (state_update)
	{
		DebugPritnF("CIAA -- ICR: %02x CRA %02x CRB %02x TIMER_A: %d / %d, TIMER_B: %d / %d, VSYNC_COUNTER: %d\n", chip_ciaa.icr,  chip_ciaa.a.cr, chip_ciaa.b.cr,
			TIMER_A,
			TIMER_A_LATCH,
			TIMER_B,
			TIMER_B_LATCH, 
			VSYNC_COUNTER );
	}
*/
}

extern int ciaa_signal;
extern struct Process *MainTask;

static UWORD CIAARead( UWORD reg, BOOL *handled, struct PUHData *pd, struct ExecBase* SysBase )
{
	UWORD	result;

//	DebugPrintF("CIAA REG: %08lx\n", reg);

	switch( reg )
	{

		case PRA:
		case 0xFE:			// think its a mirror.
			result = ciaa_pra;
			*handled = TRUE;
			break;

//***********************************************************************//

		case TALO:
			result = (TIMER_A & 0xFF);
			*handled = TRUE;
			break;

		case TAHI:
			result = (TIMER_A >> 8);
			*handled = TRUE;
			break;

//***********************************************************************//

		case TBLO:
			result = (TIMER_B & 0xFF);
			*handled = TRUE;
			break;

		case TBHI:
			result = (TIMER_B >> 8);
			*handled = TRUE;
			break;

//***********************************************************************//

		case TODLO:
			result = (VSYNC_COUNTER & 0xFF);
			*handled = TRUE;
			break;

		case TODMID:
			result = (VSYNC_COUNTER & 0xFF00) >> 8;
			*handled = TRUE;
			break;

		case TODHI:
			result = (VSYNC_COUNTER & 0xFF0000) >> 16;
			*handled = TRUE;
			break;

//***********************************************************************//

		case ICR:
			result = chip_ciaa.icr & 0xFF;
			chip_ciaa.icr = 0;
			*handled = TRUE;
			break;

		case CRA:
			result = chip_ciaa.a.cr & 0xFF;
			*handled = TRUE;
			break;

		case CRB:
			result = chip_ciaa.b.cr & 0xFF;
			*handled = TRUE;
			break;

		default:
			// Just carry out the intercepted read operation

			result = cd_ReadWord( 0x00BFD001 | reg );
			break;
	}

	return result;
}

static void CIAAWrite( UWORD reg, UWORD value, BOOL *handled, struct PUHData *pd, struct ExecBase* SysBase )
{

//	DebugPrintF("Set REG: %03x value: %02x\n", reg, value);

	switch( reg )
	{

//***********************************************************************//

		case TALO:
			TIMER_A_LATCH = (TIMER_A_LATCH & 0xFF00) | (value & 0xFF) ;
			*handled = TRUE;
			break;

		case TAHI:
			TIMER_A_LATCH = (TIMER_A_LATCH & 0x00FF) | (value & 0xFF) << 8;
			TIMER_A = TIMER_A_LATCH;		// load latch value..
			SET_CRA(START,SET);
			*handled = TRUE;
			break;

//***********************************************************************//

		case TBLO:
			TIMER_B_LATCH = (TIMER_B_LATCH & 0xFF00) | (value & 0xFF) ;
			*handled = TRUE;
			break;

		case TBHI:
			TIMER_B_LATCH = (TIMER_B_LATCH & 0x00FF) | (value & 0xFF) << 8;
			TIMER_B = TIMER_B_LATCH;		// load latch value..
			SET_CRB(START,SET);
			*handled = TRUE;
			break;

//***********************************************************************//

		case TODLO:
			VSYNC_COUNTER_LATCH = (VSYNC_COUNTER_LATCH & ~0xFF) | (value & 0xFF);
			*handled = TRUE;
			break;

		case TODMID:
			VSYNC_COUNTER_LATCH = (VSYNC_COUNTER_LATCH & ~0xFF00) | ((value & 0xFF) << 8);
			*handled = TRUE;
			break;

		case TODHI:
			VSYNC_COUNTER_LATCH = (VSYNC_COUNTER_LATCH & ~0xFF0000) | ((value & 0xFF) << 16);
			VSYNC_COUNTER = VSYNC_COUNTER_LATCH;
			*handled = TRUE;
			break;

		case CRA:
			chip_ciaa.a.cr = value & 0xFF;

			DebugPrintF("set chip_ciaa.a.cra = %d\n",value);

			if (chip_ciaa.a.cr & (1<<START)) TIMER_A = TIMER_A_LATCH;
			*handled = TRUE;
			break;

		case CRB:
			chip_ciaa.b.cr = value & 0xFF;

			DebugPrintF("set chip_ciaa.b.crb = %d\n",value);

			if (chip_ciaa.a.cr & (1<<START)) TIMER_B = TIMER_B_LATCH;
			*handled = TRUE;
			break;

//***********************************************************************//

		case ICR:

			DebugPrintF("Set ICR: %02x\n", value);
			{
				int bits = value & 0x07E;
				chip_ciaa.icr = value & 0x80 ? chip_ciaa.icr | bits : chip_ciaa.icr & ~bits;
				if (chip_ciaa.icr & 3) Signal(MainTask, ciaa_signal );	// if interupts bits are set, signal main task.
				*handled = TRUE;
			}
			break;

		default:

			cd_WriteWord( 0xBFE001 | reg , value );
			*handled = TRUE;
			break;
	}
}

ULONG CIAA(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd)
{
	struct ExecIFace *IExec = (struct ExecIFace *)pSysBase->MainInterface;
	BOOL bHandled1 = FALSE;
	BOOL bHandled2 = FALSE;
	APTR pFaultAddress;

	/* Read the faulting address */
	pFaultAddress = (APTR)pContext->dar;

		ULONG op_code  = 0;
		ULONG sub_code = 0;
		ULONG d_reg = 0;
		ULONG a_reg = 0;
		LONG	offset = 0;
		ULONG b_reg = 0;
		ULONG instruction;
		ULONG eff_addr;
		ULONG value;

		// we need to know what we can, and can't do!
		GetSystemInfo(SchedulerState_tags);

#ifdef DEBUG
		bool debug = false;

		if ( (ULONG) pFaultAddress == 0xDFF006) debug = false;

		if (debug) debug_info( pContext, pSysBase, pFaultAddress );
#endif

		instruction = *(ULONG *)pContext->ip;
		op_code = (instruction & 0xFC000000) >> 26;
		d_reg = (instruction & 0x03E00000) >> 21;
		a_reg = (instruction & 0x001F0000) >> 16;

		if (op_code == 31)
		{
			b_reg = (instruction & 0xF800) >> 11;
			sub_code = (instruction & 0x7FE)	>> 1;
		}
		else
		{
			offset = (instruction & 0xFFFF);
			if (offset > 0x8000)
			{
				offset = -65536 + offset;
			}
		}

		switch(op_code)
		{
			case 42: /* lha */
				eff_addr =(a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (int32)CIAARead((eff_addr & 0xFFE),&bHandled1,pd,pSysBase);

				if (pContext->gpr[d_reg] & 0x8000) /* signed? */
					pContext->gpr[d_reg] |= 0xFFFF0000;

				DEBUG( "lha %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 32: /* lwz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (uint32)(CIAARead((eff_addr & 0xFFE),&bHandled1,pd,pSysBase) << 16) |
												(uint32)CIAARead((eff_addr & 0xFFE) + 2,&bHandled2,pd,pSysBase);

				DEBUG( "lwz %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 40: /* lhz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (int32)CIAARead((eff_addr & 0xFFE),&bHandled1,pd,pSysBase);
				DEBUG( "lhz %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 44: /* sth */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg] & 0xffff;

				DEBUG( "sth r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIAAWrite((eff_addr & 0xFFE),value,&bHandled1,pd,SysBase);
			break;

			case 34: /* lbz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (int32)CIAARead( (eff_addr & 0xFFE),&bHandled1,pd,pSysBase) & 0xFF;
				DEBUG( "lbz r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );
				break;

			case 36: /* stw */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg];

				DEBUG( "stw r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIAAWrite((eff_addr & 0xFFE),value>>16,&bHandled1,pd,SysBase);
				CIAAWrite((eff_addr & 0xFFE)+2,value&0xffff,&bHandled2,pd,SysBase);
			break;

			case 38: /* stb */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg] & 0xFF;

//				DebugPrintF( "stb r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIAAWrite((eff_addr & 0xFFE),value,&bHandled1,pd,SysBase);
				break;

			case 31:
				switch(sub_code)
				{
					case 87: /* lbzx */

						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];

						pContext->gpr[d_reg] = (int32) CIAARead( (eff_addr & 0xFFE),&bHandled1,pd,pSysBase) & 0xFF;

						DEBUG( "lbzx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, pContext->gpr[d_reg] );

						break;

					case 407: /* sthx */
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg] & 0xffff;

						DEBUG( "sthx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIAAWrite((eff_addr & 0xFFE),value,&bHandled1,pd,SysBase);
					break;

					case 151: /* stwx */
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg];

						DEBUG( "stwx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIAAWrite((eff_addr & 0xFFE),value>>16,&bHandled1,pd,SysBase);
						CIAAWrite((eff_addr & 0xFFE)+2,value&0xffff,&bHandled2,pd,SysBase);
					break;

					case 215: /* stbx */
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg] & 0xff;

						DEBUG( "stbx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIAAWrite((eff_addr & 0xFFF),value,&bHandled1,pd,SysBase);
						break;

					case 343: /* lhax */
						eff_addr = (a_reg==0?pContext->gpr[a_reg]:0) + pContext->gpr[b_reg];

						pContext->gpr[d_reg] = (int32)CIAARead((eff_addr & 0xFFE),&bHandled1,pd,pSysBase);

						if (pContext->gpr[d_reg] & 0x8000) /* signed? */
							pContext->gpr[d_reg] |= 0xFFFF0000;

						DEBUG( "lhax r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, pContext->gpr[d_reg] );
					break;

					default:
						{
							char opcodeName[LEN_DISASSEMBLE_OPCODE_STRING];
							char operands[LEN_DISASSEMBLE_OPERANDS_STRING];
							DisassembleNative(pContext->ip, opcodeName, operands);
							DebugPrintF("*** Unhandled op_code 31 (subcode %d) :::: %s %s \n", sub_code,opcodeName,operands);
						}
						break;
				}
			break;

			default:
				DEBUG("*** Unhandled op_code %d (subcode %d)\n", op_code, sub_code);
		}

		if (bHandled1)
		{
			pContext->ip += 4;
			return TRUE;
		}
		else
		{
			DEBUG( "Didn't handle access!\n" );
		}

	/* call original handler, if we didn't handle it */
	if (!bHandled1)
	{
		return ((ULONG (*)(struct ExceptionContext *pContext, struct ExecBase *pSysBase, APTR userData))
			pd->m_OldFaultInt->is_Code)(pContext, pSysBase, pd->m_OldFaultInt->is_Data);
	}

	return TRUE;
}
