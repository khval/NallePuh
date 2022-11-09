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

#ifndef NO_GUI
#include <classes/window.h>
#include <gadgets/listbrowser.h>
#include <intuition/gadgetclass.h>
#include <libraries/resource.h>
#endif

#include <clib/alib_protos.h>
#include <proto/ahi.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/intuition.h>
#include <proto/locale.h>
#include <proto/expansion.h>
#include <proto/libblitter.h>

#ifndef NO_GUI
#include <proto/listbrowser.h>
#include <proto/resource.h>
#endif

#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NallePUH.h"
#include "PUH.h"

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
static BOOL ShowGUI( struct PUHData* pd );
static BOOL HandleGUI( Object* window, struct Gadget** gadgets, struct PUHData* pd );

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

struct DebugIFace *IDebug = NULL;
struct LibBlitterIFace *ILibBlitter = NULL;

#ifndef NO_GUI
	struct Library *ResourceBase = NULL;
	struct Library *ListBrowserBase = NULL;
#endif

struct UtilityBase *UtilityBase;

#ifdef __amigaos4__
	struct AHIIFace *IAHI;
	struct IntuitionIFace *IIntuition;
	struct LocaleIFace *ILocale;
	struct UtilityIFace *IUtility;
#endif

/******************************************************************************
** Disable ctrl-c *************************************************************
******************************************************************************/

void __chkabort( void )
{
	// Disable automatic ctrl-c handling
}


/******************************************************************************
** GUI utility functions ******************************************************
******************************************************************************/

#ifndef NO_GUI
ULONG RefreshSetGadgetAttrsA( struct Gadget *	g, struct Window *w,	struct Requester* r,	struct TagItem* tags )
{
	ULONG retval;
	BOOL	changedisabled = FALSE;
	BOOL	disabled	 = FALSE;

	if ( w != NULL )
	{
		if ( FindTagItem( GA_Disabled, tags ) )
		{
			changedisabled = TRUE;
 			disabled = g->Flags & GFLG_DISABLED;
 		}
 	}

	retval = SetGadgetAttrsA( g, w, r, tags );

	if ( w != NULL &&
			( retval != 0 || 
				( changedisabled && disabled != ( g->Flags & GFLG_DISABLED ) ) ) )
	{
		RefreshGList( g, w, r, 1 );
		retval = 1;
	}

	return retval;
}

ULONG RefreshSetGadgetAttrs( struct Gadget *g, struct Window *w, struct Requester *r, Tag tag1, ... )
{
	return RefreshSetGadgetAttrsA( g, w, r, (struct TagItem*) &tag1 );
}

struct Node*LBAddNodeA( struct Gadget *	lb,	struct Window *w,	struct Requester *r,	struct Node *n,	struct TagItem* tags )
{
	return (struct Node*) DoGadgetMethod( lb, w, r,LBM_ADDNODE, NULL, (ULONG) n, (ULONG) tags );
}

struct Node *LBAddNode( struct Gadget *lb, struct Window *w, struct Requester* r, struct Node *n, Tag tag1, ... )
{
	return (struct Node*) DoGadgetMethod( lb, w, r, LBM_ADDNODE, NULL, (ULONG) n, (ULONG) &tag1 );
}
#endif

/******************************************************************************
** main ***********************************************************************
******************************************************************************/

int main( int argc,char* argv[] )
{
	int	 rc = 0;
	BOOL	gui_mode = FALSE;

	ULONG mode_id	 = 0;
	ULONG frequency = 0;
	ULONG level = 0;

	if ( ! OpenLibs() )
	{
		return 20;
	}
#ifndef NO_GUI
	if ( argc == 1 && ResourceBase != NULL )
	{
		// Gui mode

		gui_mode = TRUE;
	}
	else
#endif
	if ( argc != 3 )
	{
		//Printf( "Usage: %s [0x]<AHI mode ID> <Frequency> <Level>\n", argv[ 0 ] );
		//Printf( "Level can be 0 (no patches), 1 (ROM patches) or 2 (appl. patches)\n" );
	 Printf( "Usage: %s [0x]<AHI mode ID> <Frequency>\n", argv[ 0 ] );
		return 10;
	}

	#ifdef __amigaos4__
	{
		struct Library	*ExpansionBase;
	 struct ExpansionIFace *IExpansion;
	 BOOL	 Classic = TRUE;

	 ExpansionBase = OpenLibrary( "expansion.library", 50 );
	 GETIFACE(Expansion);
	 if (IExpansion != NULL)
	 {
			STRPTR	extensions;

		GetMachineInfoTags(
			GMIT_Extensions, (ULONG) &extensions,
			TAG_DONE );

		if (!strstr(extensions, "classic.customchips"))
				Classic = FALSE;

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

		mode_id	 = strtol( argv[ 1 ], &mode_ptr, 0 );
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
				if ( ! ShowGUI( pd ) )
				{
					Printf( "Failed to create GUI.\n" );
					rc = 20;
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
				if ( ! InstallPUH( flags,	mode_id, frequency ) )
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

static BOOL OpenLibs( void )
{
	IntuitionBase = (struct IntuitionBase*) OpenLibrary( "intuition.library", 39 );
	LocaleBase = (struct LocaleBase*) OpenLibrary( "locale.library", 39 );
	UtilityBase = (struct UtilityBase*) OpenLibrary( "utility.library", 39 );
	LibBlitterBase = (struct Library *) OpenLibrary( "libblitter.library", 1 );

	#ifndef NO_GUI
	ResourceBase	 = OpenLibrary( "resource.library", 44 );
	ListBrowserBase = OpenLibrary( "listbrowser.gadget", 44 );
	#endif

	GETIFACE(LibBlitter);
	GETIFACE(Intuition);
	GETIFACE(Locale);
	GETIFACE(Utility);

	if (ILibBlitter) 
	{
		Printf("found & using LibBlitter.library\n");
	}
	else
	{
		Printf("NOT using LibBlitter.library, sorry\n");
	}

	if ( IntuitionBase == NULL || LocaleBase == NULL )
	{
		CloseLibs();
		return FALSE;
	}

	return TRUE;
}


/******************************************************************************
** CloseLibs ******************************************************************
******************************************************************************/

#define safe_CloseLibrary(b) if (b) { CloseLibrary( (struct Library *) b ); b = NULL;}

static void CloseLibs( void )
{
	DROPIFACE(Intuition);
	DROPIFACE(Utility);
	DROPIFACE(Locale);

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
			AHIDevice = OpenDevice( AHINAME,	AHI_NO_UNIT,(struct IORequest*) AHIio,0UL );

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

	AHIBase	 = NULL;
	AHImp = NULL;
	AHIio = NULL;
	AHIDevice = IOERR_OPENFAIL;
}


/******************************************************************************
** ShowGUI ********************************************************************
******************************************************************************/

static BOOL ShowGUI( struct PUHData* pd )
{
#ifdef NO_GUI
	return FALSE;
#else
	BOOL rc = FALSE;

	struct Catalog *catalog;
	struct Screen *screen;
	struct MsgPort *idcmp_port;
	struct MsgPort *app_port;
	RESOURCEFILE	resource;
	Object *window;
	struct Gadget **gadgets;

	catalog = OpenCatalogA( NULL, "NallePUH.catalog",NULL);
	
	screen = LockPubScreen( NULL );
	
	if ( screen != NULL )
	{
		idcmp_port = CreateMsgPort();
		
		if ( idcmp_port != NULL )
		{
			app_port = CreateMsgPort();
			
			if ( app_port != NULL )
			{
				resource = RL_OpenResource( RCTResource, screen, catalog );

				if ( resource != NULL )
				{
					window = RL_NewObject( resource, WIN_1_ID,
								 WINDOW_SharedPort, (ULONG) idcmp_port,
								 WINDOW_AppPort,		(ULONG) app_port,
								 TAG_DONE );
																 
					if ( window != NULL )
					{
						gadgets = (struct Gadget**) RL_GetObjectArray( resource, window, GROUP_2_ID );

						if ( gadgets != NULL )
						{
							DoMethod( window, WM_OPEN );
							rc = HandleGUI( window, gadgets, pd );
							DoMethod( window, WM_CLOSE);
						}
					}
					RL_CloseResource( resource );
				}
				
				DeleteMsgPort( app_port );
			}
			
			DeleteMsgPort( idcmp_port );
		}

		UnlockPubScreen( NULL, screen );
	}
	
	CloseCatalog( catalog );

	return rc;
#endif
}


/******************************************************************************
** HandleGUI ********************************************************************
******************************************************************************/

#ifndef NO_GUI

ASMCALL SAVEDS static ULONG FilterFunc( REG( a0, struct Hook *hook ),	REG( a2, struct AHIAudioModeRequester* req ), REG( a1, ULONG mode_id ) )
{
	// Remove all Paula modes (hardcoded mode IDs suck.)

	if ( ( mode_id & 0xffff0000	) == 0x00020000 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}


struct LogData
{
	struct Gadget* m_Gadget;
	struct Window* m_Window;
};


ASMCALL SAVEDS static void LogToList( REG( a0, struct Hook*hook ),REG( a2, struct PUHData* pd ), REG( a1, STRPTR message ) )
{
	struct LogData* d = (struct LogData*) hook->h_Data;

	LBAddNode( d->m_Gadget, d->m_Window, NULL,
						 (struct Node*) ~0,
						 LBNCA_CopyText, TRUE,
						 LBNA_Column,		0,
						 LBNCA_Text,		 (ULONG) message,
						 TAG_DONE );
}

static void ClearList( struct LogData* d )
{
	struct List* list;
	struct Node* node;

	GetAttr( LISTBROWSER_Labels, d->m_Gadget, (ULONG*) &list );

	// Detach
	RefreshSetGadgetAttrs( d->m_Gadget, d->m_Window, NULL, LISTBROWSER_Labels, NULL, TAG_DONE );

	// Free and remove nodes
	while( ( node = RemTail( list ) ) != NULL )
	{
		FreeListBrowserNode( node );
	}

	// Attach the (now empty) list again
	RefreshSetGadgetAttrs( d->m_Gadget, d->m_Window, NULL, LISTBROWSER_Labels, list, TAG_DONE );
}


struct TagItem *chip_tags[]=
{
	AVT_Type, MEMF_SHARED,
	AVT_Contiguous, TRUE,
	AVT_Alignment, 16,
	AVT_ClearWithValue, 0,
	TAG_END
}

static BOOL HandleGUI( Object * window,	 struct Gadget** gadgets,	 struct PUHData* pd )
{
	BOOL	rc = FALSE;
	BOOL	quit = FALSE;
	BOOL	messed_with_registers = FALSE;

	struct Window* win_ptr = NULL;
	ULONG	window_signals = 0;

	ULONG	audio_mode = 0;
	ULONG	frequency = 0;

	void *chip = NULL;
	struct Custom *custom = (struct Custom*) 0xdff000;
	struct LogData log_data;

	struct Hook log_hook =
	{
		{ NULL, NULL },
		(HOOKFUNC) LogToList,
		NULL,
		&log_data
	};

	chip = AllocVecTagList( pooh11_sblen, chip_tags );

	if ( chip == NULL )
	{
		LogPUH( pd, "Failed to alloc chip memory." );
		return FALSE;
	}

	CopyMem( pooh11_sb, chip, pooh11_sblen );

	GetAttr( WINDOW_SigMask, window, &window_signals );
	GetAttr( WINDOW_Window,	window, (ULONG*) &win_ptr );

	log_data.m_Gadget = gadgets[ GAD_MESSAGES ];
	log_data.m_Window = win_ptr;

	SetPUHLogger( &log_hook, pd );

	while( ! quit )
	{
		ULONG mask;
		
		mask = Wait( window_signals | SIGBREAKF_CTRL_C );
		
		if ( mask & SIGBREAKF_CTRL_C )
		{
			quit = TRUE;
			rc	 = TRUE;
			break;
		}

		if ( mask & window_signals )
		{
			ULONG input_flags = 0;
			UWORD code = 0;
			
			while( ( input_flags = DoMethod( window, WM_HANDLEINPUT, &code ) ) 
						 != WMHI_LASTMSG )
			{
				switch( input_flags & WMHI_CLASSMASK)
				{
					case WMHI_CLOSEWINDOW:
						quit = TRUE;
						rc	 = TRUE;
						break;

					case WMHI_ICONIFY:
						DoMethod( window, WM_ICONIFY );
						GetAttr( WINDOW_Window,	window, (ULONG*) &win_ptr );
						log_data.m_Window = win_ptr;
						break;
						
					case WMHI_UNICONIFY:
						DoMethod( window, WM_OPEN );
						GetAttr( WINDOW_Window,	window, (ULONG*) &win_ptr );
						log_data.m_Window = win_ptr;
						break;

					case WMHI_GADGETUP:
					{
						switch( input_flags & RL_GADGETMASK )
						{
							case GAD_MODE_SELECT:
							{
								struct AHIAudioModeRequester* req = NULL;
								
								struct TagItem								filter_tags[] =
								{
									{ AHIDB_Realtime,		TRUE },
									{ AHIDB_MaxChannels, 4		},
									{ TAG_DONE,					0		}
								};

								struct Hook filter_hook =
								{
									{ NULL, NULL },
									(HOOKFUNC) FilterFunc,
									NULL,
									NULL
								};

								req = AHI_AllocAudioRequest(
										AHIR_Window,		 (ULONG) win_ptr,
										AHIR_SleepWindow,	TRUE,
										AHIR_InitialAudioID,	audio_mode,
										AHIR_InitialMixFreq,	frequency,
										AHIR_DoMixFreq,	TRUE,
										AHIR_DoDefaultMode,	TRUE,
										AHIR_FilterFunc,	 (ULONG) &filter_hook,
										AHIR_FilterTags,	 (ULONG) filter_tags,
										TAG_DONE );

								if ( req == NULL )
								{
									quit = TRUE;
									rc	 = FALSE;
								}
								else
								{
									if ( AHI_AudioRequest( req, TAG_DONE ) )
									{
										char buffer[ 256 ];

										audio_mode = req->ahiam_AudioID;
										frequency = req->ahiam_MixFreq;
										
										if ( AHI_GetAudioAttrs( audio_mode, NULL,	AHIDB_BufferLen, 255, AHIDB_Name, (ULONG) buffer, TAG_DONE ) )
										{
											RefreshSetGadgetAttrs( gadgets[ GAD_MODE_INFO ], win_ptr, NULL,
													STRINGA_TextVal, (ULONG) buffer, TAG_DONE );

											RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
													GA_Disabled, FALSE,	TAG_DONE );
										}
									}

									AHI_FreeAudioRequest( req );
								}

								break;
							}

							case GAD_INSTALL:
							{
								ULONG flags = 0;
								ULONG patch_rom = 0;
								ULONG patch_apps = 0;
								ULONG toggle_led = 0;

								GetAttr( GA_Selected, gadgets[ GAD_PATCH_ROM ], &patch_rom );
								GetAttr( GA_Selected, gadgets[ GAD_PATCH_APPS ], &patch_apps );
								GetAttr( GA_Selected, gadgets[ GAD_TOGGLE_LED ], &toggle_led );

								ClearList( &log_data );

								if ( patch_rom )
								{
									flags |= PUHF_PATCH_ROM;
								}

								if ( patch_apps )
								{
									flags |= PUHF_PATCH_APPS;
								}

								if ( toggle_led )
								{
									flags |= PUHF_TOGGLE_LED;
								}

								if ( ! InstallPUH( flags,	audio_mode, frequency, pd ) )
								{
									LogPUH( pd, "Unable to install PUH." );
								}
								else
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_ROM ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_TOGGLE_LED ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_MODE_SELECT ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_UNINSTALL ], win_ptr, NULL,
											GA_Disabled, FALSE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
											GA_Disabled, FALSE,	TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
											GA_Disabled, TRUE,	TAG_DONE );
								}

								break;
							}


							case GAD_UNINSTALL:
							{
								ULONG patch_rom = 0;

								ClearList( &log_data );

								UninstallPUH( pd );

								RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_ROM ], win_ptr, NULL,
										GA_Disabled, FALSE,	TAG_DONE );

								GetAttr( GA_Selected, gadgets[ GAD_PATCH_ROM	], &patch_rom );
								
								if ( patch_rom )
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, FALSE,	TAG_DONE );
								}

								RefreshSetGadgetAttrs( gadgets[ GAD_TOGGLE_LED ], win_ptr, NULL,
										GA_Disabled, FALSE,	TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_MODE_SELECT ], win_ptr, NULL,
										GA_Disabled, FALSE,	TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
										GA_Disabled, FALSE,	TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_UNINSTALL ], win_ptr, NULL,
										GA_Disabled, TRUE,	TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
										GA_Disabled, TRUE,	TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
										GA_Disabled, TRUE,	TAG_DONE );
								break;
							}


							case GAD_ACTIVATE:
							{
								if ( ! ActivatePUH( pd ) )
								{
									LogPUH( pd, "Unable to activate PUH." );
								}
								else
								{
									LogPUH( pd, "Activated PUH." );

									RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
											GA_Disabled, TRUE,
											TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
											GA_Disabled, FALSE,
											TAG_DONE );
								}

								break;
							}


							case GAD_DEACTIVATE:
							{
								DeactivatePUH( pd );

								LogPUH( pd, "Deactivated PUH." );

								RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
										GA_Disabled, FALSE,
										TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
										GA_Disabled, TRUE,
										TAG_DONE );

								break;
							}
							
							case GAD_TEST:
							{
								messed_with_registers = TRUE;

								WriteWord( &custom->dmacon, DMAF_AUD0 );

								Delay( 1 );	// The infamous DMA-wait! ;-)

								WriteLong( &custom->aud[ 0 ].ac_ptr, (ULONG) chip );
								WriteWord( &custom->aud[ 0 ].ac_len, pooh11_sblen / 2);
								WriteWord( &custom->aud[ 0 ].ac_per, 161 );
								WriteWord( &custom->aud[ 0 ].ac_vol, 64 );

								WriteWord( &custom->dmacon, DMAF_SETCLR | DMAF_AUD0 );

								WriteWord( &custom->aud[ 0 ].ac_len, 1 );
								break;
							}
							
							case GAD_PATCH_ROM:
								if ( code )
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, FALSE,
											TAG_DONE );
								}
								else
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, TRUE,
											GA_Selected, FALSE,
											TAG_DONE );
								}
								break;
						}
						
						break;
					}

					default:
						break;
				}
			}
		}
	}
	
	SetPUHLogger( NULL, pd );

	if ( messed_with_registers )
	{
		WriteWord( &custom->dmacon, DMAF_AUD0 );
		WriteWord( &custom->aud[ 0 ].ac_vol, 0 );
	}

	FreeVec( chip );

	return rc;
}

#endif
