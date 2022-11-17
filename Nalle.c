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
#include <string.h>

#include "locale/NallePUH.h"
#include "PUH.h"
#include "debug.h"

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

static BOOL OpenLibs( void );
static void CloseLibs( void );
static BOOL OpenAHI( void );
static void CloseAHI( void );

#include "gui.h"

/******************************************************************************
** Global variables ***********************************************************
******************************************************************************/

static struct MsgPort *AHImp = NULL;
static struct AHIRequest *AHIio = NULL;
static BYTE AHIDevice = IOERR_OPENFAIL;

struct Library * AHIBase = NULL;
struct IntuitionBase* IntuitionBase = NULL;
struct Library *LocaleBase = NULL;
struct Library *MMUBase = NULL;
struct Library *LibBlitterBase = NULL;
struct Library *DiskfontBase = NULL;

struct Library *IconBase = NULL;
struct Library *WorkbenchBase = NULL;
struct Library *ApplicationBase = NULL;

struct DebugIFace *IDebug = NULL;
struct LibBlitterIFace *ILibBlitter = NULL;

struct Library *StringBase = NULL;
struct Library *LayoutBase = NULL;
struct Library *LabelBase = NULL;
struct Library *ChooserBase = NULL;
struct Library *IntegerBase = NULL;
struct Library *ListBrowserBase = NULL;
struct Library *ClickTabBase = NULL;
struct Library *WindowBase = NULL;
struct Library *CheckBoxBase = NULL;
struct Library *RequesterBase = NULL;


struct StringIFace *IString = NULL;
struct LayoutIFace *ILayout = NULL;
struct LabelIFace *ILabel = NULL;
struct ChooserIFace *IChooser = NULL;
struct IntegerIFace *IInteger = NULL;
struct ListBrowserIFace *IListBrowser = NULL;
struct ClickTabIFace *IClickTab = NULL;
struct WindowIFace *IWindow = NULL;
struct CheckBoxIFace *ICheckBox = NULL;
struct RequesterIFace *IRequester = NULL;
struct DiskfontIFace *IDiskfont = NULL;

struct IconIFace * IIcon = NULL;
struct WorkbenchIFace *IWorkbench = NULL;

struct Catalog *catalog = NULL;
struct Locale *_locale = NULL;
ULONG *codeset_page = NULL;

struct UtilityBase *UtilityBase;

struct AHIIFace *IAHI = NULL;
struct IntuitionIFace *IIntuition = NULL;
struct LocaleIFace *ILocale = NULL;
struct UtilityIFace *IUtility = NULL;
struct ApplicationIFace *IApplication = NULL;

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

int main( int argc,char* argv[] )
{
	int	rc = 0;

	ULONG mode_id	= 0;
	ULONG frequency = 0;
	ULONG level = 0;

	if ( argc == 0)
	{
		// wb startup
		gui_mode = TRUE;
		cli_start = FALSE;

		if ( ! OpenLibs() )
		{
			CloseLibs();
			return 20;
		}
	}
	else 	if ( argc == 1 )
	{
		gui_mode = TRUE;

		if ( ! OpenLibs() )
		{
			CloseLibs();
			return 20;
		}
	}
	else if ( argc != 3 )
	{
		//Printf( "Usage: %s [0x]<AHI mode ID> <Frequency> <Level>\n", argv[ 0 ] );
		//Printf( "Level can be 0 (no patches), 1 (ROM patches) or 2 (appl. patches)\n" );
		Printf( "Usage: %s [0x]<AHI mode ID> <Frequency>\n", argv[ 0 ] );
		return 10;
	}
	else	// default CLI startup
	{
		if ( ! OpenLibs() )
		{
			CloseLibs();
			return 20;
		}
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
			return 10;
		}

		if ( level > 2 )
		{
			Printf( "Invalid value for Level.\n" );
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
** OpenLibs *******************************************************************
******************************************************************************/

BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = OpenLibrary( name , ver);

	if (*base)
	{
		 *interface = GetInterface( *base,  iname , iver, TAG_END );
		if (!*interface) printf("Unable to getInterface %s for %s %d!\n",iname,name,ver);
	}
	else
	{
	   	printf("Unable to open the %s %d!\n",name,ver);
	}
	return (*interface) ? TRUE : FALSE;
}


static BOOL OpenLibs( void )
{
	IntuitionBase = (struct IntuitionBase*) OpenLibrary( "intuition.library", 39 );
	LocaleBase = (struct LocaleBase*) OpenLibrary( "locale.library", 39 );
	UtilityBase = (struct UtilityBase*) OpenLibrary( "utility.library", 39 );
	LibBlitterBase = (struct Library *) OpenLibrary( "libblitter.library", 1 );

	GETIFACE(LibBlitter);
	GETIFACE(Intuition);
	GETIFACE(Locale);
	GETIFACE(Utility);

	if ( ! open_lib( "string.gadget", 53, "main", 1, &StringBase, (struct Interface **) &IString) ) return FALSE;
	if ( ! open_lib( "layout.gadget", 53, "main", 1, &LayoutBase, (struct Interface **) &ILayout) ) return FALSE;
	if ( ! open_lib( "label.image", 53, "main", 1, &LabelBase, (struct Interface **) &ILabel) ) return FALSE;
	if ( ! open_lib( "chooser.gadget", 53, "main", 1, &ChooserBase, (struct Interface **) &IChooser) ) return FALSE;
	if ( ! open_lib( "integer.gadget", 53, "main", 1, &IntegerBase, (struct Interface **) &IInteger) ) return FALSE;
	if ( ! open_lib( "listbrowser.gadget", 53, "main", 1, &ListBrowserBase, (struct Interface **) &IListBrowser) ) return FALSE;
	if ( ! open_lib( "clicktab.gadget", 53, "main", 1, &ClickTabBase, (struct Interface **) &IClickTab) ) return FALSE;
	if ( ! open_lib( "window.class", 53, "main", 1, &WindowBase, (struct Interface **) &IWindow) ) return FALSE;
	if ( ! open_lib( "checkbox.gadget", 53, "main", 1, &CheckBoxBase, (struct Interface **) &ICheckBox) ) return FALSE;
	if ( ! open_lib( "requester.class", 53, "main", 1, &RequesterBase, (struct Interface **) &IRequester) ) return FALSE;
	if ( ! open_lib( "locale.library", 53 , "main", 1, &LocaleBase, (struct Interface **) &ILocale  ) ) return FALSE;
	if ( ! open_lib( "diskfont.library", 53 , "main", 1, &DiskfontBase, (struct Interface **) &IDiskfont ) ) return FALSE;
	if ( ! open_lib( "icon.library", 53 , "main", 1, &IconBase, (struct Interface **) &IIcon ) ) return FALSE;
	if ( ! open_lib( "workbench.library", 53 , "main", 1, &WorkbenchBase, (struct Interface **) &IWorkbench ) ) return FALSE;
	if ( ! open_lib( "application.library", 53 , "application", 2, &ApplicationBase, (struct Interface **) &IApplication ) ) return FALSE;

	if (ILibBlitter) 
	{
		if (cli_start) Printf("found & using Libblitter.library\n");
	}
	else
	{
		if (cli_start) Printf(" Libblitter.library, not found sorry\n");
	}

	if ( IntuitionBase == NULL || LocaleBase == NULL )
	{
		CloseLibs();
		return FALSE;
	}

	_locale = (struct Locale *) OpenLocale(NULL);

	if (_locale)
	{
		codeset_page = (ULONG *) ObtainCharsetInfo(DFCS_NUMBER, (ULONG) _locale -> loc_CodeSet , DFCS_MAPTABLE);
	}

	catalog = OpenCatalog(NULL, "basilisk.catalog", OC_BuiltInLanguage, "english", TAG_DONE);

	appID = FindApplication(FINDAPP_Name, "NallePuh", TAG_END);
	if (appID != 0)
	{
		appID = 0;	// we did not create this appid.
		return FALSE;	// if already started.
	}

	if ( !( appID = RegisterApplication("NallePuh",REGAPP_LoadPrefs, FALSE,TAG_DONE)) ) return FALSE;

	return TRUE;
}


/******************************************************************************
** CloseLibs ******************************************************************
******************************************************************************/

#define safe_CloseLibrary(b) if (b) { CloseLibrary( (struct Library *) b ); b = NULL;}

static void CloseClasses( void )
{
	if (StringBase) CloseLibrary(StringBase); StringBase = 0;
	if (IString) DropInterface((struct Interface*) IString); IString = 0;

	if (LayoutBase) CloseLibrary(LayoutBase); LayoutBase = 0;
	if (ILayout) DropInterface((struct Interface*) ILayout); ILayout = 0;

	if (LabelBase) CloseLibrary(LabelBase); LabelBase = 0;
	if (ILabel) DropInterface((struct Interface*) ILabel); ILabel = 0;

	if (ChooserBase) CloseLibrary(ChooserBase); ChooserBase = 0;
	if (IChooser) DropInterface((struct Interface*) IChooser); IChooser = 0;

	if (IntegerBase) CloseLibrary(IntegerBase); IntegerBase = 0;
	if (IInteger) DropInterface((struct Interface*) IInteger); IInteger = 0;

	if (ListBrowserBase) CloseLibrary(ListBrowserBase); ListBrowserBase = 0;
	if (IListBrowser) DropInterface((struct Interface*) IListBrowser); IListBrowser = 0;

	if (ClickTabBase) CloseLibrary(ClickTabBase); ClickTabBase = 0;
	if (IClickTab) DropInterface((struct Interface*) IClickTab); IClickTab = 0;

	if (WindowBase) CloseLibrary(WindowBase); WindowBase = 0;
	if (IWindow) DropInterface((struct Interface*) IWindow); IWindow = 0;

	if (ClickTabBase) CloseLibrary(ClickTabBase); ClickTabBase = 0;
	if (IClickTab) DropInterface((struct Interface*) IClickTab); IClickTab = 0;

	if (CheckBoxBase) CloseLibrary(CheckBoxBase); CheckBoxBase = 0;
	if (ICheckBox) DropInterface((struct Interface*) ICheckBox); ICheckBox = 0;

	if (RequesterBase) CloseLibrary(RequesterBase); RequesterBase = 0;
	if (IRequester) DropInterface((struct Interface*) IRequester); IRequester = 0;

	if (DiskfontBase) CloseLibrary(DiskfontBase); DiskfontBase = 0;
	if (IDiskfont) DropInterface((struct Interface*) IDiskfont); IDiskfont = 0;

	if (IconBase) CloseLibrary(IconBase); IconBase = 0;
	if (IIcon) DropInterface((struct Interface*) IIcon); IIcon = 0;

	if (WorkbenchBase) CloseLibrary(WorkbenchBase); WorkbenchBase = 0;
	if (IWorkbench) DropInterface((struct Interface*) IWorkbench); IWorkbench = 0;
}

static void CloseLibs( void )
{
	if (appID)
	{
		UnregisterApplication(appID, NULL);
		appID = 0;
	}

	if (ILocale)	// check if lib is open...
	{
		if (catalog)
		{
			CloseCatalog(catalog); 
			catalog = NULL;
		}
	
		if (_locale)
		{
			CloseLocale(_locale); 
			_locale = NULL;
		}
	}

	CloseClasses();

	DROPIFACE(Application);
	DROPIFACE(Intuition);
	DROPIFACE(Utility);
	DROPIFACE(Locale);

	safe_CloseLibrary(ApplicationBase);
	safe_CloseLibrary(LibBlitterBase);
	safe_CloseLibrary(UtilityBase);
	safe_CloseLibrary(IntuitionBase);
	safe_CloseLibrary(LocaleBase);
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



