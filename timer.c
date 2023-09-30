
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <devices/timer.h>

#include "PUH.h"
#include "emu_cia.h"

struct MsgPort *timer_port = NULL;
struct TimeRequest *timer_io = NULL;
ULONG  timer_mask = 0;

extern struct options options;

extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

extern void update_timer_ciaa();
extern void update_timer_ciab();

extern struct TagItem SchedulerState_tags[];

void open_timer( void )
{
	timer_port = (struct MsgPort*) AllocSysObjectTags(ASOT_PORT, TAG_DONE);

	if (timer_port)
	{
		timer_io = (struct TimeRequest *) AllocSysObjectTags(ASOT_IOREQUEST, 
						ASOIOR_ReplyPort , timer_port, 
						ASOIOR_Size, sizeof(struct TimeRequest), 
						TAG_END);

		if (timer_io)
		{
			if (!OpenDevice( (char *) TIMERNAME, UNIT_MICROHZ, (struct IORequest *)timer_io, 0))
			{
				timer_mask = 1 << timer_port->mp_SigBit;
				timer_io->Request.io_Command = TR_ADDREQUEST;
				timer_io->Time.Seconds = 0;
				timer_io->Time.Microseconds = 16667 ;
				SendIO((struct IORequest *)timer_io);
			}
		}
	}
}

void close_timer()
{

	// Stop timer
	if (timer_io)
	{

		if (!CheckIO((struct IORequest *)timer_io)) AbortIO((struct IORequest *)timer_io);
		WaitIO((struct IORequest *)timer_io);
		CloseDevice((struct IORequest *)timer_io);


		FreeSysObject(ASOT_IOREQUEST,timer_io);
		timer_io = NULL;
	}

	if (timer_port)	FreeSysObject(ASOT_PORT,timer_port);
	timer_port = NULL;

}

void handel_timer( void )
{
	// Restart timer
	timer_io->Request.io_Command = TR_ADDREQUEST;
	timer_io->Time.Seconds = 0;
	timer_io->Time.Microseconds = 16667;
	SendIO((struct IORequest *)timer_io);

	if (options.activated)
	{
//		printf("%02x, %02x, %02x, %02x\n", chip_ciaa.a.cr, chip_ciaa.b.cr, chip_ciab.a.cr,  chip_ciab.b.cr);

		GetSystemInfo(SchedulerState_tags);

		if ( (chip_ciaa.a.cr  |  chip_ciaa.b.cr)  & 1)  update_timer_ciaa();
		if ( (chip_ciab.a.cr  |  chip_ciab.b.cr)  & 1)  update_timer_ciab();
	}
}

