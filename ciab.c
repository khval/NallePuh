
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
#include <devices/ahi.h>
#include <exec/emulation.h>
#include <graphics/gfxbase.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include <proto/exec.h>

#include "PUH.h"
#include "emu_cia.h"

extern struct TagItem SchedulerState_tags[];
extern void debug_info( struct ExceptionContext *pContext, struct ExecBase *pSysBase, APTR pFaultAddress );

static uint32 HORIZONTAL_SYNC_COUNTER = 0;
static uint32 HORIZONTAL_SYNC_COUNTER_LATCH = 0;

struct chip chip_ciab ;

#define SET_CRA(bit,set) SET_REG_BIT(chip_ciab.a.cr,bit,set)
#define SET_CRB(bit,set) SET_REG_BIT(chip_ciab.a.cr,bit,set)

static uint32 TIMER_A_us = 0;
static uint32 TIMER_B_us = 0;

#define TIMER_A chip_ciab.a.ticks
#define TIMER_A_LATCH chip_ciab.a.ticks_latch
#define TIMER_A_ticks chip_ciab.a.new_ticks

#define TIMER_B chip_ciab.b.ticks
#define TIMER_B_LATCH chip_ciab.b.ticks_latch
#define TIMER_B_ticks chip_ciab.b.new_ticks

static uint32 TIMER_H_us = 0;
static uint32 TIMER_H_ticks = 0;

static bool te_start_set = false;
struct timeval cia_te_start ;

extern struct Process *MainTask;
extern uint32 SchedulerState;

extern UWORD cd_ReadWord( void* address );
extern void cd_WriteWord( void* address, UWORD value );

static void cia_get_usec( int *out_usec )
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
}

void update_timer_ciab()
{
	int us = 0;

	// calulate delta time... from last read..

	cia_get_usec( &us );

	// time us accumulators, us is subtracted when number is dividable.

	// convert delta time in us to ticks
	
	if (INMODE_A(chip_ciab.icr) == 0)
	{
		TIMER_A_us += us;
		if ( (TIMER_A) && us2ticks( CIA_TIMER_UNIT, &TIMER_A_us , &TIMER_A_ticks ) )  	// count down mode, single shot... :-P
		{
			do_cia_timer_a( &chip_ciab);
		}
	}

	if (INMODE_B(chip_ciab.icr) == 1)
	{
		TIMER_B_us += us;
		if ( (TIMER_B) && us2ticks( CIA_TIMER_UNIT, &TIMER_B_us, &TIMER_B_ticks ) )	        // count down mode, single shot... :-P
		{
			do_cia_timer_b( &chip_ciab);
		}
	}

	TIMER_H_us += us;
	if ( us2ticks( HORIZONTAL_SYNC_HZ, &TIMER_H_us, &TIMER_H_ticks ) )	        // count down mode, single shot... :-P
	{
		HORIZONTAL_SYNC_COUNTER = (HORIZONTAL_SYNC_COUNTER + TIMER_H_ticks) & 0xFFFFFF;
	}
}

static UWORD CIABRead( UWORD reg, BOOL *handled, struct PUHData *pd, struct ExecBase* SysBase )
{
	UWORD	result;

	switch( reg )
	{

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
			result = (HORIZONTAL_SYNC_COUNTER & 0xFF);
			*handled = TRUE;
			break;

		case TODMID:
			result = (HORIZONTAL_SYNC_COUNTER & 0xFF00) >> 8;
			*handled = TRUE;
			break;

		case TODHI:
			result = (HORIZONTAL_SYNC_COUNTER & 0xFF0000) >> 16;
			*handled = TRUE;
			break;

//***********************************************************************//

		case ICR:
			result = chip_ciab.icr & 0xFF;
			chip_ciab.icr = 0;
			*handled = TRUE;
			break;

		case CRA:
			result = chip_ciab.a.cr & 0xFF;
			*handled = TRUE;
			break;

		case CRB:
			result = chip_ciab.b.cr & 0xFF;
			*handled = TRUE;
			break;

//***********************************************************************//

		default:
			// Just carry out the intercepted read operation

			result = cd_ReadWord( (void *) (0x00BFD000 | reg) );
			break;
	}

	return result;
}


static void CIABWrite( UWORD reg, UWORD value, BOOL *handled, struct PUHData *pd, struct ExecBase* SysBase )
{
	UWORD* address = (UWORD*) ( (ULONG) pd->m_CustomDirect + reg );


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
			HORIZONTAL_SYNC_COUNTER_LATCH = (HORIZONTAL_SYNC_COUNTER_LATCH & ~0xFF) | (value & 0xFF);
			*handled = TRUE;
			break;

		case TODMID:
			HORIZONTAL_SYNC_COUNTER_LATCH = (HORIZONTAL_SYNC_COUNTER_LATCH & ~0xFF00) | ((value & 0xFF) << 8);
			*handled = TRUE;
			break;

		case TODHI:
			HORIZONTAL_SYNC_COUNTER_LATCH = (HORIZONTAL_SYNC_COUNTER_LATCH & ~0xFF0000) | ((value & 0xFF) << 16);
			HORIZONTAL_SYNC_COUNTER = HORIZONTAL_SYNC_COUNTER_LATCH;
			*handled = TRUE;
			break;

		case CRA:
			chip_ciab.a.cr = value & 0xFF;

			DebugPrintF("chip_ciab.a.cr = %d\n",value);

			if (chip_ciab.a.cr & (1<<START)) TIMER_A = TIMER_A_LATCH;
			*handled = TRUE;
			break;

		case CRB:
			chip_ciab.b.cr = value & 0xFF;

			DebugPrintF("chip_ciab.b.cr = %d\n",value);

			if (chip_ciab.a.cr & (1<<START)) TIMER_B = TIMER_B_LATCH;
			*handled = TRUE;
			break;

//***********************************************************************//

		case ICR:
			{
				int bits = value & 0x07E;
				chip_ciab.icr = value & 0x80 ? chip_ciab.icr | bits : chip_ciab.icr & ~bits;
				if (chip_ciab.icr & 3) Signal(MainTask, chip_ciab.signal );	// if interupts bits are set, signal main task.
				*handled = TRUE;
			}
			break;

		default:

			cd_WriteWord( address, value );
			*handled = TRUE;
			break;
	}
}


ULONG CIAB(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd)
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

//		DebugPrintF("op_code: %d\n",op_code);

		switch(op_code)
		{
			case 42: /* lha */
				eff_addr =(a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (int32)CIABRead((eff_addr & 0xFFF),&bHandled1,pd,pSysBase);

				if (pContext->gpr[d_reg] & 0x8000) /* signed? */
					pContext->gpr[d_reg] |= 0xFFFF0000;

				DEBUG( "lha %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 32: /* lwz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (uint32)(CIABRead((eff_addr & 0xFFF),&bHandled1,pd,pSysBase) << 16) |
												(uint32)CIABRead((eff_addr & 0xFFF) + 2,&bHandled2,pd,pSysBase);

				DEBUG( "lwz %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 40: /* lhz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				pContext->gpr[d_reg] = (int32)CIABRead((eff_addr & 0xFFF),&bHandled1,pd,pSysBase);
				DEBUG( "lhz %lx, %lx\n", eff_addr, pContext->gpr[d_reg] );
			break;

			case 44: /* sth */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg] & 0xffff;

				DEBUG( "sth r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIABWrite((eff_addr & 0xFFF),value,&bHandled1,pd,SysBase);
			break;

			case 34: /* lbz */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;

				{
					int d_mask = (1<<5)-1;
					d_reg = (instruction >> 21) & d_mask ;
				}

				value = (int32)CIABRead( (eff_addr & 0xFFF),&bHandled1,pd,pSysBase) & 0xFF;

				pContext->gpr[d_reg] = value;

				DEBUG( "lbz r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );
				break;

			case 36: /* stw */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg];

				DEBUG( "stw r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIABWrite((eff_addr & 0xFFF),value>>16,&bHandled1,pd,SysBase);
				CIABWrite((eff_addr & 0xFFF)+2,value&0xffff,&bHandled2,pd,SysBase);
			break;

			case 38: /* stb */
				eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + offset;
				value = pContext->gpr[d_reg] & 0xFF;

				DEBUG( "stb r%ld,%ld(r%ld) (ea: %lx	data: %lx)\n", d_reg, offset, a_reg, eff_addr, value );

				CIABWrite((eff_addr & 0xFFF),value,&bHandled1,pd,SysBase);
			break;


			case 31:
				switch(sub_code)
				{
					case 87: /* lbzx */

						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];

						pContext->gpr[d_reg] = (int32) CIABRead( (eff_addr & 0xFFF),&bHandled1,pd,pSysBase) & 0xFF;

						DEBUG( "lbzx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, pContext->gpr[d_reg] );

						break;

					case 407: /* sthx */
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg] & 0xffff;

						DEBUG( "sthx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIABWrite((eff_addr & 0xFFF),value,&bHandled1,pd,SysBase);
					break;

					case 151: /* stwx */
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg];

						DEBUG( "stwx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIABWrite((eff_addr & 0xFFF),value>>16,&bHandled1,pd,SysBase);
						CIABWrite((eff_addr & 0xFFF)+2,value&0xffff,&bHandled2,pd,SysBase);
					break;

					case 215: // stbx //
						eff_addr = (a_reg==0?0:pContext->gpr[a_reg]) + pContext->gpr[b_reg];
						value = pContext->gpr[d_reg] & 0xff;

						DEBUG( "stbx r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, value );

						CIABWrite((eff_addr & 0xFFF),value,&bHandled1,pd,SysBase);
						break;

					case 343: /* lhax */
						eff_addr = (a_reg==0?pContext->gpr[a_reg]:0) + pContext->gpr[b_reg];

						pContext->gpr[d_reg] = (int32)CIABRead((eff_addr & 0xFFF),&bHandled1,pd,pSysBase);

						if (pContext->gpr[d_reg] & 0x8000) /* signed? */
							pContext->gpr[d_reg] |= 0xFFFF0000;

						DEBUG( "lhax r%ld, r%ld, r%ld (ea: %lx	data: %lx)\n", d_reg, a_reg, b_reg, eff_addr, pContext->gpr[d_reg] );
					break;

					default:
						{
							char opcodeName[LEN_DISASSEMBLE_OPCODE_STRING];
							char operands[LEN_DISASSEMBLE_OPERANDS_STRING];
							DisassembleNative( (char *) pContext->ip, opcodeName, operands);
							DebugPrintF("*** Unhandled op_code 31 (subcode %d) :::: %s %s \n", sub_code,opcodeName,operands);
						}
						break;
				}
			break;

			default:
				DebugPrintF("*** Unhandled op_code %d (subcode %d)\n", op_code, sub_code);
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

