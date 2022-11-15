
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <classes/window.h>
#include <gadgets/listbrowser.h>
#include <intuition/gadgetclass.h>
#include <libraries/resource.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/listbrowser.h>
#include <proto/resource.h>
#include <proto/locale.h>
#include <proto/intuition.h>
#include <proto/ahi.h>

#include <devices/ahi.h>

#include <hardware/custom.h>
#include <hardware/dmabits.h>

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#define CATCOMP_ARRAY

#include "locale/NallePUH.c"
#include "PUH.h"
#include "gui.h"

#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>
#include <proto/window.h>
#include <proto/integer.h>
#include <proto/layout.h>
#include <proto/button.h>
#include <proto/string.h>
#include <proto/label.h>
#include <inline4/label.h>
#include <images/label.h>

#include "reaction_macros.h"

extern struct Custom CustomData;

enum
{
	win_prefs,
	win_end
};

struct Window *	win[win_end];
Object *			layout[win_end];
Object *			obj[ID_END];

extern UBYTE pooh11_sb[];
extern struct Catalog *catalog;

struct kIcon
{
	struct Gadeget *gadget ;
	struct Image *image ;
};

struct options
{
	BOOL installed;
	BOOL activated;
} options;

void init_options( struct options *o)
{
	o -> installed = FALSE;
	o -> activated = FALSE;
}

struct kIcon iconifyIcon = { NULL, NULL };

const char *_L_default(LONG num) 
{
	unsigned int n;
	int ret = 0;

	for (n=0;n< ( sizeof(CatCompArray) / sizeof( struct CatCompArrayType ) );n++)
	{
		if (CatCompArray[n].cca_ID == num) { ret = n; break; }
	}

	return CatCompArray[ret].cca_Str ;
}

const char *_L_catalog(unsigned int num) 
{
	const char *str;
	str = GetCatalogStr(catalog, num, NULL);
	if (!str) return _L_default(num) ;
	return str;
}

const char *(*_L)(unsigned int num) = _L_default;

/******************************************************************************
** ShowGUI ********************************************************************
******************************************************************************/

char window_title_name[100];

extern struct MsgPort *iconifyPort ;

extern void enable_Iconify();
extern void disable_Iconify();

void handel_iconify()
{
	struct Message *msg;
	BOOL disabled = FALSE;

	enable_Iconify();
	if (iconifyPort)
	{
		do
		{
			Wait( 1L << iconifyPort -> mp_SigBit);

			while ((msg = (struct Message *) GetMsg( iconifyPort ) ))
			{
				ReplyMsg( (struct Message*) msg );
				disabled = TRUE;
			}
		} while (disabled == FALSE );
	}

	if (disabled)
	{
		disable_Iconify();
	}
}

void update_gui( int win_nr, struct rc *rc )
{

	switch (win_nr)
	{
		case win_prefs:
			{
				if (rc)
				{
					char tmp[100];

					sprintf(tmp,"%08x", rc -> audio_mode );
					RefreshSetGadgetAttrs( obj[ GAD_MODE_ID ], win[ win_nr ], NULL,
						STRINGA_TextVal, (ULONG) tmp, 
						TAG_DONE );

					RefreshSetGadgetAttrs( obj[ GAD_MODE_INFO ], win[ win_nr ], NULL,
						STRINGA_TextVal, (ULONG) rc -> AHI_name, 
						TAG_DONE );
				}

				RefreshSetGadgetAttrs( obj[ GAD_SELECT_MODE ], win[ win_nr ], NULL,
					GA_Disabled,  options.activated ? TRUE : FALSE, 
					TAG_DONE );
	
				RefreshSetGadgetAttrs( obj[ GAD_ACTIVATE ], win[ win_nr ], NULL,
					GA_Disabled, options.activated ? TRUE : FALSE, 
					TAG_DONE );

				RefreshSetGadgetAttrs( obj[ GAD_DEACTIVATE ], win[ win_nr ], NULL,
					GA_Disabled, options.activated ? FALSE : TRUE, 
					TAG_DONE );

				RefreshSetGadgetAttrs( obj[ GAD_TEST ], win[ win_nr ], NULL,
					GA_Disabled, options.activated ? FALSE : TRUE, 
					TAG_DONE );
			}
			break;
	}
}

void init_prefs(int win_nr)
{

	sprintf(window_title_name,_L(msgNallePUH),99,99);

	layout[win_nr] = (Object*) WindowObject,
			WA_Title,       window_title_name,
			WA_ScreenTitle, window_title_name,

			WA_SizeGadget,   TRUE,
			WA_DepthGadget,  TRUE,
			WA_DragBar,      TRUE,
			WA_CloseGadget,  TRUE,
			WA_Activate,     TRUE,
			WA_SmartRefresh, TRUE,
			WA_Width, 400,
			WINDOW_IconifyGadget, TRUE,
			WINDOW_Iconifiable, TRUE,
			WINDOW_Position, WPOS_CENTERSCREEN,
			WINDOW_ParentGroup, VLayoutObject,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_DeferLayout, TRUE,

				LAYOUT_AddChild, VGroupObject,

					LAYOUT_AddChild, MakeString(GAD_MODE_ID),
					CHILD_Label, MakeLabel(GAD_MODE_ID),

					LAYOUT_AddChild, MakeString(GAD_MODE_INFO),
					CHILD_Label, MakeLabel(GAD_MODE_INFO),

					LAYOUT_AddChild, MakeButton(GAD_SELECT_MODE),

				LayoutEnd,
				CHILD_WeightedHeight, 0,

				LAYOUT_AddChild, HGroupObject,

					LAYOUT_AddChild, MakeButton(GAD_ACTIVATE),
					LAYOUT_AddChild, MakeButton(GAD_DEACTIVATE),
					LAYOUT_AddChild, MakeButton(GAD_TEST),
					LAYOUT_AddChild, MakeButton(GAD_ABOUT),


				LayoutEnd,
//				CHILD_WeightedWidth, 0,
				CHILD_WeightedHeight, 0,

			EndMember,
		EndWindow;
}

void close_window(int layout_nr)
{
	if (layout[ layout_nr ])
	{
		DisposeObject( (Object *) layout[ layout_nr ] );
		layout[ layout_nr ]	= 0;
		win[ layout_nr ]	= 0;
	}
}

BOOL open_window(ULONG win_id )
{
	win[ win_id ] = RA_OpenWindow( layout[win_id] );
	if ( ! win[ win_id ]) return FALSE;

	switch ( win_id )
	{
		case win_prefs:
			RSetAttrO( win_prefs, GAD_MODE_ID, GA_Disabled, TRUE);
			RSetAttrO( win_prefs, GAD_MODE_INFO, GA_Disabled, TRUE);
			break;
	}

	update_gui( win_id, NULL );
	return TRUE;
}

struct rc ShowGUI( struct PUHData* pd )
{
	struct rc rc;
	struct Screen *screen;
	struct MsgPort *idcmp_port;
	struct MsgPort *app_port;
	
	rc.rc = FALSE;
	screen = LockPubScreen( NULL );
	
	if ( screen != NULL )
	{
		idcmp_port = (struct MsgPort *) AllocSysObjectTags(ASOT_PORT, TAG_DONE);
		
		if ( idcmp_port != NULL )
		{
			app_port = (struct MsgPort *) AllocSysObjectTags(ASOT_PORT, TAG_DONE);
			
			if ( app_port != NULL )
			{
				init_prefs(win_prefs);

				if (open_window( win_prefs ))
				{
					rc = HandleGUI( win[ win_prefs ], pd );
					close_window(win_prefs);
				}
			
				FreeSysObject( ASOT_PORT, app_port );
			}
			
			FreeSysObject( ASOT_PORT, idcmp_port );
		}

		UnlockPubScreen( NULL, screen );
	}
	
	return rc;
}

/******************************************************************************
** HandleGUI ********************************************************************
******************************************************************************/

ULONG FilterFunc( REG( a0, struct Hook *hook ), REG( a2, struct AHIAudioModeRequester* req ), REG( a1, ULONG mode_id ) )
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


void LogToList( REG( a0, struct Hook*hook ),REG( a2, struct PUHData* pd ), REG( a1, STRPTR message ) )
{
#if 0
	struct LogData* d = (struct LogData*) hook->h_Data;
	LBAddNode( d->m_Gadget, d->m_Window, NULL,
						(struct Node*) ~0,
						LBNCA_CopyText, TRUE,
						LBNA_Column, 	0,
						LBNCA_Text, 	(ULONG) message,
						TAG_DONE );
#endif
}

/*
void ClearList( struct LogData* d )
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
*/


void nallepuh_test()
{
	char *wave;
	ULONG n;
	BOOL bHandled;
	ULONG wave_size = 512;

	wave = (char *) malloc( wave_size );

	for (n=0;n<wave_size;n++)
	{
		wave[n] = sin( (float) n / 500.0f ) * 126;
	}

	emu_WriteWord( &bHandled, &CustomData.dmacon, DMAF_AUD0 );
	Delay( 1 );	// The infamous DMA-wait! ;-)
	emu_WriteLong( &bHandled, &CustomData.aud[ 0 ].ac_ptr, (ULONG) wave );
	emu_WriteWord( &bHandled, &CustomData.aud[ 0 ].ac_len, wave_size / 2);
	emu_WriteWord( &bHandled, &CustomData.aud[ 0 ].ac_per, 161 );
	emu_WriteWord( &bHandled, &CustomData.aud[ 0 ].ac_vol, 64 );
	emu_WriteWord( &bHandled, &CustomData.dmacon, DMAF_SETCLR | DMAF_AUD0 );
	emu_WriteWord( &bHandled, &CustomData.aud[ 0 ].ac_len, 1 );

	Wait(5);

	FreeVec(wave);

}

void init_rc(struct rc *rc, struct Window * window, struct PUHData* pd)
{
	rc -> rc = FALSE;
	rc -> quit = FALSE;
	rc -> messed_with_registers = FALSE;
	rc -> audio_mode = 0;
	rc -> frequency = 0;
	rc -> win_ptr = NULL;
	rc -> code = 0;
	rc -> win_ptr = window;
	rc -> pd = pd;
	rc -> AHI_name[0] = 0;
}

void HandleGadgets(ULONG input_flags , struct rc *rc)
{
	switch( input_flags & RL_GADGETMASK )
	{
		case GAD_SELECT_MODE:
			{
				struct AHIAudioModeRequester* req = NULL;
								
				struct TagItem								filter_tags[] =
				{
					{ AHIDB_Realtime, 	TRUE },
					{ AHIDB_MaxChannels, 4		},
					{ TAG_DONE, 				0		}
				};

				struct Hook filter_hook =
				{
					{ NULL, NULL },
					(HOOKFUNC) FilterFunc,
					NULL,
					NULL
				};

				req = AHI_AllocAudioRequest(
						AHIR_Window, 	(ULONG) rc -> win_ptr,
						AHIR_SleepWindow, TRUE,
						AHIR_InitialAudioID, rc -> audio_mode,
						AHIR_InitialMixFreq, rc -> frequency,
						AHIR_DoMixFreq, TRUE,
						AHIR_DoDefaultMode, TRUE,
						AHIR_FilterFunc, (ULONG) &filter_hook,
						AHIR_FilterTags, (ULONG) filter_tags,
						TAG_DONE );

				if ( req == NULL )
				{
					rc->quit = TRUE;
					rc->rc	= FALSE;
				}
				else
				{
					if ( AHI_AudioRequest( req, TAG_DONE ) )
					{
						rc -> audio_mode = req->ahiam_AudioID;
						rc -> frequency = req->ahiam_MixFreq;
										
						if ( AHI_GetAudioAttrs( rc -> audio_mode, NULL, AHIDB_BufferLen, 255, AHIDB_Name, (ULONG) rc -> AHI_name, TAG_DONE ) )
						{
							update_gui( win_prefs, rc );
						}
					}

					AHI_FreeAudioRequest( req );
				}
				break;
			}

		case GAD_ACTIVATE:

			{
				if ( ! InstallPUH( 0, rc -> audio_mode, rc -> frequency ) )
				{
					DebugPrintF( "Can't open AHI & set exception" );		// maybe use a requestor!!!
				}
				else
				{
					options.installed = TRUE;

					if (  ActivatePUH( rc -> pd ) )
					{
						options.activated = TRUE;
						update_gui( win_prefs, NULL );
					}

					update_gui( win_prefs, NULL );
				}
			}
			break;

		case GAD_DEACTIVATE:
			{
				UninstallPUH( rc -> pd );

				options.activated = FALSE;
				update_gui( win_prefs, NULL );
			}
			break;

							
		case GAD_TEST:
			{
				rc -> messed_with_registers = TRUE;
				nallepuh_test();
			}
			break;
	}
}



struct rc HandleGUI( struct Window * window, struct PUHData* pd )
{
	struct rc rc;
	ULONG	window_signals = 0;

	init_rc( &rc, window, pd );
	init_options( &options );

	window_signals = 1L << window -> UserPort -> mp_SigBit;

//	log_data.m_Gadget = obj[ GAD_MESSAGES ];
//	log_data.m_Window = rc.win_ptr;
//	SetPUHLogger( &log_hook, pd );

	while( ! rc.quit )
	{
		ULONG mask;
		
		mask = Wait( window_signals | SIGBREAKF_CTRL_C );
		
		if ( mask & SIGBREAKF_CTRL_C )
		{
			rc.quit = TRUE;
			rc.rc	= TRUE;
			break;
		}

		if ( mask & window_signals )
		{
			ULONG input_flags = 0;
			
			while( ( input_flags = RA_HandleInput( layout[ win_prefs ] ,&rc.code) ) != WMHI_LASTMSG )
			{
				switch( input_flags & WMHI_CLASSMASK)
				{
					case WMHI_CLOSEWINDOW:
						rc.quit = TRUE;
						rc.rc	= TRUE;
						break;

					case WMHI_ICONIFY:

						// empty event queue
						while( ( input_flags = RA_HandleInput( layout[ win_prefs ] ,&rc.code) ) != WMHI_LASTMSG );

						close_window( win_prefs );

						handel_iconify();

						init_prefs(win_prefs);			// we trashed objs with close_window :-)
						if (open_window(win_prefs))
						{
							window = win[ win_prefs ];
							window = rc.win_ptr;
							rc.log_data.m_Window = rc.win_ptr;
							update_gui( win_prefs, &rc );
						}
						else 
						{
							printf("failed !!\n");
							return rc;
						}
						break;

					case WMHI_GADGETUP:
						HandleGadgets(input_flags, &rc);
						break;

					default:
						break;
				}
			}
		}
	}
	
//	SetPUHLogger( NULL, pd );

	if ( rc.messed_with_registers )
	{
		BOOL bHandled;
		emu_WriteWord( &bHandled, &CustomData.dmacon, DMAF_AUD0 );
		emu_WriteWord( &bHandled, &CustomData.aud[ 0 ].ac_vol, 0 );
	}

	return rc;
}
