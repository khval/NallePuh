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

#include "locale/NallePUH.h"
#include "PUH.h"
#include "debug.h"
#include "emu_cia.h"

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


extern int req(const char *title,const  char *body,const char *buttons, ULONG image);

int main( int argc,char* argv[] )
{
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
		req("No chipram","you should change setpatch command in startup sequene to:\n\nSetPatch QUIET WAITFORVALIDATE ADDCHIPRAM=2\n","OK", 0);
	}

	if ( argc == 0)
	{
		// wb startup
		gui_mode = TRUE;
		cli_start = FALSE;
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

				rcode = ShowGUI( pd );

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



