
/*
	This file is Copyright (C) 2023: Kjetil Hvalstrand
	MIT License.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/execbase.h>
#include <exec/memory.h>
#include <exec/resident.h>
#include <exec/emulation.h>

#include <hardware/custom.h>
#include <hardware/intbits.h>

#include "emu_cia.h"

extern struct Process *MainTask;

int cia_frequency_select = 0;
int HORIZONTAL_SYNC_HZ = 0;
int VSYNC_HZ = 0;

#define scale_factor 1.0f

double CIA_TIMER_UNIT = scale_factor;

#define VSYNC_60HZ 16670
#define VSYNC_50HZ 20000

#define HORIZONTAL_SYNC_60HZ VSYNC_60HZ / MAX_SCANLINES
#define HORIZONTAL_SYNC_50HZ VSYNC_50HZ / MAX_SCANLINES

extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

extern struct TagItem SchedulerState_tags[];

extern struct Custom CustomData;

void update_hz()
{
	switch (cia_frequency_select)
	{
		case 0: 	// PAL mode 50HZ 
			CIA_TIMER_UNIT = scale_factor / 1.41f;
			HORIZONTAL_SYNC_HZ = HORIZONTAL_SYNC_50HZ;
			VSYNC_HZ = VSYNC_50HZ;
			break;

		case 1: 	// NTSC mode 60HZ 
			CIA_TIMER_UNIT = scale_factor / 1.397f;
			HORIZONTAL_SYNC_HZ = HORIZONTAL_SYNC_60HZ;
			VSYNC_HZ = VSYNC_50HZ;
			break;
	}
}

bool us2ticks( double unit, uint32 *us, uint32 *ticks )
{
	if ( (double) *us > unit)
	{
		int newTicks = *us / unit;
		*us -= (int)  ( (double) newTicks  * unit);

//		DebugPrintF("new ticks: %d\n", newTicks);

		*ticks += newTicks;
		return true;
	}
	return false;
}

static void init_chip_timer( struct cia_timer *timer )
{
	timer -> ticks = 0;
	timer -> ticks_latch = 0;
}

void init_chip( const char *name, struct chip *chip, ULONG sig, ULONG irq )
{
	unsigned int n;
	init_chip_timer( &chip  -> a );
	init_chip_timer( &chip  -> b );
	chip -> interrupt_mask = 0;

	for (n=0;n<2;n++)
	{
		chip -> interrupts[n] = NULL;
	}

	chip -> signal = 1L << sig ;
	chip -> icr = 0;

	for (n = 0; n<sizeof(chip -> icr_handle);n++) chip -> icr_handle[n] = 0;
	chip -> irq = irq;
	chip -> name = name;
}

#define RUNMODE 3
#define INMODE 5

#define IS_COUNT_IN(reg) reg & 1<<INMODE
#define IS_ONE_SHOT(reg) reg & 1<<RUNMODE

#define overflow_bit_a 0
#define overflow_bit_b 1


void do_cia_timer_a(struct chip *chip )
{
	int tmp;
	struct cia_timer *timer = &chip -> a;

	tmp = timer -> ticks - timer -> new_ticks;
	timer -> new_ticks = 0;

	if (tmp <=0)
	{
		if (IS_ONE_SHOT(chip -> icr))
		{
			SET_REG_BIT( timer->cr , START,CLR);
			tmp = 0;	
		}
		else	// CONTINUOUS MODE
		{
			tmp = timer -> ticks_latch + (tmp % timer -> ticks_latch);		// this is the same as (tmp = max - overflow)
		}

		switch (INMODE_B(chip -> a.cr))
		{
			case 2: //  Timer B counts Timer A underflow pulses
				chip -> b.new_ticks ++;
				do_cia_timer_b(chip);
				break;

			case 3: //  Timer B counts Timer A underflow pulses, while CNT pin is held high.
				chip -> b.new_ticks ++;
				do_cia_timer_b(chip);
				break;
		}

		chip -> icr |= (1<<overflow_bit_a);
		chip -> icr_handle[overflow_bit_a] = 1;
		Signal(MainTask, chip -> signal );
	}

	timer -> ticks = tmp;
}

void do_cia_timer_b(struct chip *chip)
{
	int tmp;
	struct cia_timer *timer = &chip -> b;

	tmp = timer -> ticks - timer -> new_ticks;
	timer -> new_ticks = 0;

	if (tmp <=0)
	{
		if (IS_ONE_SHOT(chip -> b.cr))
		{
			SET_REG_BIT( timer->cr , START,CLR);
			tmp = 0;	
		}
		else	// CONTINUOUS MODE
		{
			tmp = timer -> ticks_latch + (tmp % timer -> ticks_latch);		// this is the same as (tmp = max - overflow)
		}

		chip -> icr |= (1<<overflow_bit_b);
		chip -> icr_handle[overflow_bit_b] = 1;
		Signal(MainTask, chip -> signal );
	}

	timer -> ticks = tmp;
}

extern ULONG CallInt(UWORD irq, UWORD mask, struct ExceptionContext *pContext, struct ExecBase *SysBase);

void call_int( int mask, struct Interrupt *is )
{
	if ( is == NULL ) return;							// NO INTERRUPT !!!..
	if ( is->is_Code == NULL ) return;					// has no code;

//	printf("*** Calling interrupt %s at %08x\n", is -> is_Node.ln_Name ? is -> is_Node.ln_Name : "NoName" , is->is_Code );

	if (IsNative(is->is_Code))
	{
		Cause( is );
	}
	else
	{
		ULONG result = EmulateTags( is->is_Code,
			ET_SaveParamRegs, TRUE,
			ET_SuperState,	TRUE,
			ET_RegisterA0, 0x00DFF000,
			ET_RegisterD1, mask,
			ET_RegisterA1, is->is_Data,
			ET_RegisterA5, is->is_Code,
			ET_RegisterA6, SysBase,
			TAG_DONE );
	}
}

void event_chip( struct chip *chip )
{
	int b;

	// ports , External INT6
	// bit is (IRQ-1)

	if ( CustomData.intenar & (1L<<(chip -> irq-1)) ) 	// if bit is enabled !!!
	{
		Printf("%s:%ld - IRQ: %ld\n",__FUNCTION__,__LINE__, chip -> irq);

		// interrupt bit (not irq number)
		CallInt(chip -> irq - 1, 0, NULL, SysBase);
	}

	for (b=0;b<3;b++)
	{
		if ( chip -> icr_handle[b] )
		{
			chip -> icr_handle[b] = 0;

			if (chip -> interrupts[b])
				Printf("%s:%ld - %s cause interrupts %ld\n", __FUNCTION__,__LINE__,chip->name,b);
			else
				Printf("%s:%ld - %s interrupts %d is NULL\n", __FUNCTION__,__LINE__,chip->name,b);

			call_int( chip -> icr, chip -> interrupts[b] );
		}
	}
}

void dump_chip_interrupts( struct chip *chip )
{
	int b;
	const char *timers[]={"A","B"};

	for (b=0;b<2;b++)
	{
		Printf("%s: Timer %s, %08lx\n", chip->name,timers[b], chip -> interrupts[b]);
	}	
}


void event_cia( ULONG mask)
{
	if (mask & (chip_ciaa.signal | chip_ciab.signal))
	{
		GetSystemInfo(SchedulerState_tags);
	}

	// recover from forbid state.. do timeing here.

	if ( mask & chip_ciaa.signal )
	{
		printf("overflow CIAA\n");
		event_chip( &chip_ciaa );
	}

	if ( mask & chip_ciab.signal )
	{
		printf("overflow CIAB\n");
		event_chip( &chip_ciab );
	}
}

