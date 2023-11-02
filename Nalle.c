/* $Id: Nalle.c,v 1.15 2001/05/04 08:43:33 lcs Exp $ */

/*
		NallePUH -- Paula utan henne -- A minimal Paula emulator.
		Copyright (C) 2001 Martin Blom <martin@blom.org>
		
		This program is free software; you can redistribute it and/or
		modify it under the terms of the GNU General Public License
		as published by the Free Software Foundation; either version 2
		of the License, or (at your option) any later version.
		
		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
		GNU General Public License for more details.
		
		You should have received a copy of the GNU General Public License
		along with this program; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef __amigaos4__
#include "CompilerSpecific.h"
#endif

#include <devices/ahi.h>
#include <exec/errors.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <clib/alib_protos.h>
#include <proto/ahi.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/expansion.h>
#include <proto/libblitter.h>
#include <proto/diskfont.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/application.h>
#include <diskfont/diskfonttag.h>
#include <workbench/startup.h>

#include <gadgets/integer.h>
#include <gadgets/layout.h>
#include <gadgets/button.h>
#include <images/label.h>

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#define CATCOMP_ARRAY

#include "locale/NallePUH.h"

#include "PUH.h"
#include "debug.h"
#include "emu_cia.h"
#include "spawn.h"

extern bool arg_iconify;
extern bool arg_activate;

/* Version Tag */
#include "nallepuh_rev.h"
STATIC CONST UBYTE USED verstag[] = VERSTAG;

#ifdef __amigaos4__
#define GETIFACE(x)	if (x ## Base)  { I ## x = (struct x ## IFace *) GetInterface((struct Library *) x ## Base, "main", 1L, NULL); }
#define DROPIFACE(x)	if (I ## x) { DropInterface((struct Interface *) I ## x); I ## x = NULL; }
#else
#define GETIFACE(x)
#define DROPIFACE(x)
#endif

extern UBYTE pooh11_sb[];
extern ULONG pooh11_sblen;

extern BOOL OpenLibs( void );
extern void CloseLibs( void );

static BOOL OpenAHI( void );
static void CloseAHI( void );

#include "gui.h"

/******************************************************************************
** Global variables ***********************************************************
******************************************************************************/

static struct MsgPort *AHImp = NULL;
static struct AHIRequest *AHIio = NULL;
static BYTE AHIDevice = IOERR_OPENFAIL;

struct Process *cia_process = NULL;

BOOL cli_start = TRUE;
BOOL gui_mode = FALSE;

uint32 appID = 0;

/******************************************************************************
** Disable ctrl-c *************************************************************
******************************************************************************/

void __chkabort( void )
{
	// Disable automatic ctrl-c handling
}

/******************************************************************************
** main ***********************************************************************
******************************************************************************/

//extern struct chip chip_ciaa ;
//extern struct chip chip_ciab ;

bool nallepuh_quit = false;

extern int req(const char *title,const  char *body,const char *buttons, ULONG image);

extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

int ciaa_signal = -1;
int ciab_signal = -1;

extern ULONG timer_mask;
extern void open_timer( void );
extern void close_timer( void );
extern void handel_timer( void );

void activate( struct rc *rc );

void cia_fn ()
{
	ULONG mask;

	ciaa_signal = AllocSignal(-1);
	ciab_signal = AllocSignal(-1);

	init_chip( "CIAA", &chip_ciaa, ciaa_signal, 4 ); 	// hw irq 2, sw irq 4 
	init_chip( "CIAB", &chip_ciab, ciab_signal, 14 );	// hw irq 6, sw irq 14

	open_timer();

	for(;;)
	{
		mask = Wait(SIGBREAKF_CTRL_C | timer_mask | chip_ciaa.signal | chip_ciab.signal);

		if (mask & timer_mask) handel_timer();
		if (mask & SIGBREAKF_CTRL_C) break;

		event_cia( mask );
	}

	close_timer();

	if (ciaa_signal > -1) FreeSignal(ciaa_signal);
	if (ciab_signal > -1) FreeSignal(ciab_signal);
}

extern struct options options;
extern void init_options( struct options *o);

bool wbStartup(struct WBStartup *wbmsg);

int main( int argc,char* argv[] )
{
	BPTR cia_out ;

	int	rc = 0;

	ULONG mode_id	= 0;
	ULONG frequency = 0;
	ULONG level = 0;

	init_qfind_lookup();

	if ( ! OpenLibs() )
	{
		CloseLibs();
		return 20;
	}

	if (AvailMem(MEMF_TOTAL|MEMF_CHIP) == 0)
	{
		req(_L(win_no_chip_ram),_L(str_no_chip_ram),_L(req_ok), 0);
	}

	if ( argc == 0)
	{
		// wb startup
		gui_mode = TRUE;
		cli_start = FALSE;

		wbStartup( (struct WBStartup *) argv );
	}
	else 	if ( argc == 1 )
	{
		gui_mode = TRUE;
	}
	else if ( argc != 3 )
	{
		//Printf( "Usage: %s [0x]<AHI mode ID> <Frequency> <Level>\n", argv[ 0 ] );
		//Printf( "Level can be 0 (no patches), 1 (ROM patches) or 2 (appl. patches)\n" );
		Printf( "Usage: %s [0x]<AHI mode ID> <Frequency>\n", argv[ 0 ] );

		CloseLibs();
		return 10;
	}
	else	// default CLI startup
	{

	}

	#ifdef __amigaos4__
	{
		struct Library	*ExpansionBase;
		struct ExpansionIFace *IExpansion;
		BOOL	Classic = FALSE;

		ExpansionBase = OpenLibrary( "expansion.library", 50 );
		GETIFACE(Expansion);
		if (IExpansion != NULL)
		{
			STRPTR	extensions;
			GetMachineInfoTags(GMIT_Extensions, (ULONG) &extensions,TAG_DONE );
			if (strstr(extensions, "classic.customchips")) Classic = TRUE;
			DROPIFACE(Expansion);
		}

		CloseLibrary(ExpansionBase);

		if (Classic)
		{
			Printf( "Sorry, this program doesn't work on classic hardware\n" );
			CloseLibs();
			return 10;
		}
	}
	#endif

	cia_out = Open("CON:Debug",MODE_NEWFILE);

	cia_process = spawn( cia_fn, "NallePuh cia process", cia_out );

	if ( ! gui_mode )
	{
		char* mode_ptr;
		char* freq_ptr;
		char* levl_ptr;

		mode_id	= strtol( argv[ 1 ], &mode_ptr, 0 );
		frequency = strtol( argv[ 2 ], &freq_ptr, 0 );
		level = strtol( "0" /*argv[ 3 ]*/, &levl_ptr, 0 );
	
		if ( *mode_ptr != 0 || *freq_ptr != 0 || *levl_ptr != 0 )
		{
			Printf( "All arguments must be numbers.\n" );
			CloseLibs();
			return 10;
		}

		if ( level > 2 )
		{
			Printf( "Invalid value for Level.\n" );
			CloseLibs();
			return 10;
		}
	}

	if ( ! OpenAHI() )
	{
		Printf( "Unable to open ahi.device version 4.\n" );
		rc = 20;
	}

	if ( rc == 0 )
	{
		struct PUHData* pd;

		pd = AllocPUH();
		
		if ( pd == NULL )
		{
			rc = 20;
		}
		else
		{
			if ( gui_mode )
			{
				struct rc rcode;

				init_rc( &rcode, pd );
				init_options( &options );

				load("progdir:NallePuh.cfg",&rcode);

				if (arg_activate) activate( &rcode );
				if (arg_iconify)	handel_iconify();

				ShowGUI( pd, &rcode );

				if ( ! rcode.rc )
				{
					Printf( "Failed to create GUI.\n" );
					rc = 20;
				}
				else
				{
					mode_id	= rcode.audio_mode;
					frequency = rcode.frequency;
				}
			}
			else
			{
				ULONG flags = 0;

				LogPUH( pd, "Using mode ID 0x%08lx, %ld Hz.", mode_id, frequency );
#if 0
				switch( level )
				{
					case 0:
						LogPUH( pd, "No patches." );

						flags = PUHF_NONE;
						break;

					case 1:
						LogPUH( pd, "ROM patches." );

						flags = PUHF_PATCH_ROM;
						break;

					case 2:
						LogPUH( pd, "ROM and application patches." );

						flags = PUHF_PATCH_ROM | PUHF_PATCH_APPS;
						break;
				}
#endif
				if ( ! InstallPUH( flags, mode_id, frequency ) )
				{
					rc = 20;
				}
				else
				{
					if ( ! ActivatePUH( pd ) )
					{
						rc = 20;
					}
					else
					{
						LogPUH( pd, "Waiting for CTRL-C..." );
						Wait( SIGBREAKF_CTRL_C );
						LogPUH( pd, "Got it." );
				
						DeactivatePUH( pd );
					}
				
					UninstallPUH( pd );
				}
			}

			FreePUH( pd );
		}
	}

	if (cia_process) Signal(cia_process, SIGBREAKF_CTRL_C );

	CloseAHI();
	CloseLibs();

	return rc;
}



/******************************************************************************
** OpenAHI ********************************************************************
******************************************************************************/

/* Opens and initializes the device. */

static BOOL OpenAHI( void )
{
	BOOL rc = FALSE;

	AHImp = (struct MsgPort *) AllocSysObjectTags(ASOT_PORT, TAG_DONE);

	if ( AHImp != NULL )
	{
		AHIio = (struct AHIRequest*) AllocSysObjectTags(ASOT_IOREQUEST,
					ASOIOR_Size, sizeof( struct AHIRequest ), 
					ASOIOR_ReplyPort, AHImp,
					TAG_DONE);

		if ( AHIio != NULL ) 
		{
			AHIio->ahir_Version = 4;
			AHIDevice = OpenDevice( AHINAME, AHI_NO_UNIT,(struct IORequest*) AHIio,0UL );

			if ( AHIDevice == 0 )
			{
				AHIBase = (struct Library *) AHIio->ahir_Std.io_Device;
				GETIFACE(AHI);
				rc = TRUE;
			}
		}
	}

	return rc;
}


/******************************************************************************
** CloseAHI *******************************************************************
******************************************************************************/

/* Closes the device, cleans up. */

static void CloseAHI( void )
{
	if ( AHIDevice == 0 )
	{
		DROPIFACE(AHI);
		CloseDevice( (struct IORequest*) AHIio );
	}

	FreeSysObject(ASOT_IOREQUEST, AHIio );
	FreeSysObject(ASOT_PORT, AHImp );

	AHIBase	= NULL;
	AHImp = NULL;
	AHIio = NULL;
	AHIDevice = IOERR_OPENFAIL;
}



