
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <devices/timer.h>

#include "PUH.h"
#include "emu_cia.h"

extern struct TagItem SchedulerState_tags[];

struct MsgPort *timer_port = NULL;
struct TimeRequest *timer_io = NULL;

struct MsgPort *refresh_timer_port = NULL;
struct TimeRequest *refresh_timer_io = NULL;

ULONG timer_mask = 0;
ULONG refresh_signal = 0;

extern struct options options;

extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

extern void update_timer_ciaa();
extern void update_timer_ciab();

ULONG  _open_timer( struct MsgPort **timer_port, struct TimeRequest **timer_io, ULONG Seconds, ULONG Microseconds );
void _close_timer( struct MsgPort **timer_port, struct TimeRequest **timer_io );

void open_timer( void )
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	timer_mask = _open_timer(  &timer_port, &timer_io, 0 , 16667 );
}

void close_timer( void )
{
//	printf("%s:%d\n",__FUNCTION__,__LINE__);
	_close_timer( &timer_port, &timer_io );
}

void open_refresh_timer( void )
{
//	printf("%s:%d\n",__FUNCTION__,__LINE__);
	refresh_signal = _open_timer(  &refresh_timer_port, &refresh_timer_io, 2 , 0 );
}

void close_refresh_timer( void )
{
	printf("%s:%d\n",__FUNCTION__,__LINE__);
	_close_timer( &refresh_timer_port, &refresh_timer_io );
}

ULONG  _open_timer( struct MsgPort **timer_port, struct TimeRequest **timer_io, ULONG Seconds, ULONG Microseconds )
{
	ULONG signal_mask = 0;	

	*timer_port = (struct MsgPort*) AllocSysObjectTags(ASOT_PORT, TAG_DONE);

	if (*timer_port)
	{
		*timer_io = (struct TimeRequest *) AllocSysObjectTags(ASOT_IOREQUEST, 
						ASOIOR_ReplyPort , *timer_port, 
						ASOIOR_Size, sizeof(struct TimeRequest), 
						TAG_END);

		if (*timer_io)
		{
			if (!OpenDevice( (char *) TIMERNAME, UNIT_MICROHZ, (struct IORequest *) *timer_io, 0))
			{
				signal_mask = 1L << (*timer_port)->mp_SigBit;
				(*timer_io)->Request.io_Command = TR_ADDREQUEST;
				(*timer_io)->Time.Seconds = 0;
				(*timer_io)->Time.Microseconds = 16667 ;
				SendIO((struct IORequest *) *timer_io);
			}
		}
	}

	return signal_mask;
}

void _close_timer( struct MsgPort **timer_port, struct TimeRequest **timer_io)
{

	// Stop timer
	if (*timer_io)
	{

		if (!CheckIO((struct IORequest *)*timer_io)) AbortIO((struct IORequest *)*timer_io);
		WaitIO((struct IORequest *)*timer_io);
		CloseDevice((struct IORequest *)*timer_io);


		FreeSysObject(ASOT_IOREQUEST,*timer_io);
		*timer_io = NULL;
	}

	if (*timer_port)	FreeSysObject(ASOT_PORT,*timer_port);
	*timer_port = NULL;

}

extern void IO_BUTTONS_UP( void );

void handel_timer( void )
{
	// Restart timer
	timer_io->Request.io_Command = TR_ADDREQUEST;
	timer_io->Time.Seconds = 0;
	timer_io->Time.Microseconds = 16667;
	SendIO((struct IORequest *)timer_io);

	if (options.activated)
	{
		printf("%02x, %02x, %02x, %02x\n", chip_ciaa.a.cr, chip_ciaa.b.cr, chip_ciab.a.cr,  chip_ciab.b.cr);

		GetSystemInfo(SchedulerState_tags);

		if ( (chip_ciaa.a.cr  |  chip_ciaa.b.cr)  & 1)  update_timer_ciaa();
		if ( (chip_ciab.a.cr  |  chip_ciab.b.cr)  & 1)  update_timer_ciab();
	}

	IO_BUTTONS_UP( );
}

#define TIMER_A chip -> a.ticks
#define TIMER_A_LATCH chip -> a.ticks_latch
#define TIMER_A_ticks chip -> a.new_ticks

#define TIMER_B chip -> b.ticks
#define TIMER_B_LATCH chip -> b.ticks_latch
#define TIMER_B_ticks chip -> b.new_ticks


void cia_status( const char *name, struct chip *chip)
{
		Printf("%s -- chip_ciab.icr: %02lx chip_ciab.a.cr %02lx chip_ciab.b.cr %02lx TIMER_A: %ld / %ld, TIMER_B: %ld / %ld\n", 

				name,

				chip_ciab.icr, chip_ciab.a.cr, chip_ciab.b.cr,

				TIMER_A,
				TIMER_A_LATCH,

				TIMER_B,
				TIMER_B_LATCH);
}

void reactivate_refresh_timer()
{
	// Restart timer
	refresh_timer_io->Request.io_Command = TR_ADDREQUEST;
	refresh_timer_io->Time.Seconds = 2;
	refresh_timer_io->Time.Microseconds = 0;
	SendIO((struct IORequest *)refresh_timer_io);


	if (options.activated)
	{
		printf("%02x, %02x, %02x, %02x\n", chip_ciaa.a.cr, chip_ciaa.b.cr, chip_ciab.a.cr,  chip_ciab.b.cr);
		if ( (chip_ciaa.a.cr  |  chip_ciaa.b.cr)  & 1) cia_status( "CIAA", &chip_ciaa );
		if ( (chip_ciab.a.cr  |  chip_ciab.b.cr)  & 1) cia_status( "CIAB", &chip_ciab );

	}
}

