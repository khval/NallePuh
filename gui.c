
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
#include <proto/utility.h>
#include <proto/ahi.h>

#include <exec/emulation.h>

#include <devices/ahi.h>

#include <hardware/custom.h>
#include <hardware/dmabits.h>

#define CATCOMP_NUMBERS
#define CATCOMP_STRINGS
#define CATCOMP_ARRAY

#include "NallePUH_rev.h"
#include "locale/NallePUH.c"
#include "PUH.h"
#include "gui.h"
#include "file.h"

#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <proto/clicktab.h>
#include <gadgets/clicktab.h>
#include <proto/chooser.h>
#include <gadgets/chooser.h>

#include <proto/window.h>
#include <proto/integer.h>
#include <proto/layout.h>
#include <proto/button.h>
#include <proto/string.h>
#include <proto/label.h>
#include <inline4/label.h>
#include <images/label.h>

#include "reaction_macros.h"
#include "nallepuh_rev.h"
#include "emu_cia.h"

extern int req(const char *title,const  char *body,const char *buttons, ULONG image);

STATIC CONST UBYTE USED verstag[] = VERSTAG;

extern struct Custom CustomData;

extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

extern void reactivate_refresh_timer();

extern struct TagItem SchedulerState_tags[];

// workaround for reaction sucks!!
struct timeval button_press_time[4];

enum
{
	win_prefs,
	win_end
};

#if GUI_DEBUG
#undef GUI_DEBUG
#define GUI_DEBUG(a,b) printf("%s:%d\n",a,b);
#else
#define GUI_DEBUG(a,b)
#endif

struct Window *	win[win_end];
Object *			layout[win_end];
Object *			obj[ID_END];

extern UBYTE pooh11_sb[];
extern struct Catalog *catalog;

extern int cia_frequency_select;

int blitter_selected = 0;

struct kIcon
{
	struct Gadeget *gadget ;
	struct Image *image ;
};

struct options options;

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

extern ULONG timer_mask;
extern ULONG refresh_signal;

extern void handel_timer( void ); 

extern void event_cia( ULONG mask );

ULONG local_Chips[]=
{
	LIST_Chips_Paula,
	LIST_Chips_CIA,
	LIST_Chips_Status,
	0
};

CONST_STRPTR Chips
		[	sizeof(local_Chips)		/ sizeof(ULONG)	];


CONST_STRPTR frequency_names[] =
		{
			"50Hz (PAL)",
			"60Hz (NTSC)",
			NULL
		};

ULONG blitter_lib_id[4];
char *blitter_names[4];

void handel_iconify()
{
	struct Message *msg;
	BOOL disabled = FALSE;

	enable_Iconify();
	if (iconifyPort)
	{
		ULONG mask;
		int iconifyPort_signal = 1L << iconifyPort -> mp_SigBit;

		do
		{
			mask =Wait( iconifyPort_signal );

			event_cia( mask);

			if (mask & iconifyPort_signal)
			{
				while ((msg = (struct Message *) GetMsg( iconifyPort ) ))
				{
					ReplyMsg( (struct Message*) msg );
					disabled = TRUE;
				}
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
	GUI_DEBUG(__FUNCTION__,__LINE__);

	switch (win_nr)
	{
		case win_prefs:
			{
				ULONG *i;
				ULONG read_only_gadgets[]=
					{
						GAD_CIAA_CR,
						GAD_CIAA_TA,
						GAD_CIAA_TB,
						GAD_CIAB_CR,
						GAD_CIAB_TA,
						GAD_CIAB_TB,
						~0
					};

				ULONG disabled_while_active_gadgets[]=
					{
						GAD_SELECT_MODE,
						GAD_ACTIVATE,
						~0
					};

				ULONG activate_while_active_gadgets[]=
					{
						GAD_TEST,
						GAD_DEACTIVATE,
						GAD_JOY1_BUTTON1,
						GAD_JOY1_BUTTON2,
						GAD_JOY2_BUTTON1,
						GAD_JOY2_BUTTON2,
						~0
					};


				if (rc)
				{
					char tmp[100];

					sprintf(tmp,"%08lx", rc -> audio_mode );
					RefreshSetGadgetAttrs( obj[ GAD_MODE_ID ], win[ win_nr ], NULL,
						STRINGA_TextVal, (ULONG) tmp, TAG_DONE );

					RefreshSetGadgetAttrs( obj[ GAD_MODE_INFO ], win[ win_nr ], NULL,
						STRINGA_TextVal, (ULONG) rc -> AHI_name, TAG_DONE );
				}

				RefreshSetGadgetAttrs( obj[ LIST_Frequency ], win[ win_nr ], NULL,
					CHOOSER_Selected , cia_frequency_select,
					TAG_DONE);

				for (i = disabled_while_active_gadgets; *i != (ULONG) (~0); i++)
				{
					RefreshSetGadgetAttrs( obj[ *i ], win[ win_nr ], NULL,
						GA_Disabled,  options.activated ? TRUE : FALSE, TAG_DONE );
				}

				for (i = activate_while_active_gadgets; *i != (ULONG) (~0); i++)
				{
					RefreshSetGadgetAttrs( obj[ *i ], win[ win_nr ], NULL,
						GA_Disabled,  options.activated ? FALSE : TRUE, TAG_DONE );
				}

				for (i = read_only_gadgets; *i != (ULONG) (~0); i++)
				{
					RefreshSetGadgetAttrs( obj[ *i ], win[ win_nr ], NULL,
						GA_ReadOnly, TRUE, TAG_DONE );
				}
			}
			break;
	}
}

void init_STRPTR_list( ULONG *local_array, CONST_STRPTR *str_array )
{
	ULONG *local_item;
	CONST_STRPTR *str_array_item = str_array;

	for (local_item = local_array; *local_item ; local_item++, str_array_item++ )
	{
		*str_array_item = _L( (unsigned int) *local_item );
	}
	*str_array_item = NULL;

}

void init_prefs(int win_nr)
{
	sprintf(window_title_name,"%s (V%d.%d)",_L(win_title),VERSION,REVISION);

	init_STRPTR_list( local_Chips, Chips );

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
			WA_IDCMP, IDCMP_INTUITICKS | IDCMP_CLOSEWINDOW |  IDCMP_GADGETDOWN | IDCMP_GADGETUP ,

			WINDOW_IconifyGadget, TRUE,
			WINDOW_Iconifiable, TRUE,
			WINDOW_Position, WPOS_CENTERSCREEN,

			WINDOW_ParentGroup, VLayoutObject,
				LAYOUT_SpaceOuter, TRUE,
				LAYOUT_DeferLayout, TRUE,

				LAYOUT_AddChild,  ClickTabObject,
					GA_RelVerify, TRUE,
					GA_Text, Chips,
					GA_ID, PAGES_CHIP_ID ,

					CLICKTAB_PageGroup,  PageObject,

						LAYOUT_DeferLayout, TRUE,

#include "gui_paula.c"
#include "gui_cia.c"
#include "gui_status.c"

					PageEnd,

				ClickTabEnd,

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

BOOL open_window(ULONG win_id, struct rc *rc )
{
	win[ win_id ] = RA_OpenWindow( layout[win_id] );
	if ( ! win[ win_id ]) return FALSE;

	switch ( win_id )
	{
		case win_prefs:
			RSetAttrO( win_prefs, GAD_MODE_ID, GA_ReadOnly, TRUE);
			RSetAttrO( win_prefs, GAD_MODE_INFO, GA_ReadOnly, TRUE);
			RSetAttrO( win_prefs, LIST_Frequency, CHOOSER_Selected , cia_frequency_select);
			break;
	}

	update_gui( win_id, NULL );
	return TRUE;
}

struct rc ShowGUI( struct PUHData* pd,	 struct rc *rc )
{

	struct Screen *screen;
	struct MsgPort *idcmp_port;
	struct MsgPort *app_port;
	
	rc -> rc = FALSE;
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

				if (open_window( win_prefs, rc ))
				{
					HandleGUI( win[ win_prefs ], pd, rc );
					close_window(win_prefs);
				}
			
				FreeSysObject( ASOT_PORT, app_port );
			}
			
			FreeSysObject( ASOT_PORT, idcmp_port );
		}

		UnlockPubScreen( NULL, screen );
	}
	
	return *rc;
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

#define cust_addr(x) (void *) (0xDFF000 | offsetof(struct Custom,x))

void nallepuh_test()
{
	char *wave;
	ULONG n;
	BOOL bHandled;
	ULONG wave_size = 4096;

	wave = (char *) malloc( wave_size );

	if (wave)
	{
		for (n=0;n<wave_size;n++)
		{
			wave[n] = sin( 2.0 *M_PI * (float) n / 10.0f ) * 126;
		}

		emu_WriteWord( &bHandled, cust_addr(dmacon), DMAF_AUD0 );
		Delay( 1 );	// The infamous DMA-wait! ;-)
		emu_WriteLong( &bHandled, cust_addr(aud[ 0 ].ac_ptr), (ULONG) wave );
		emu_WriteWord( &bHandled, cust_addr(aud[ 0 ].ac_len), wave_size / 2);
		emu_WriteWord( &bHandled, cust_addr(aud[ 0 ].ac_per), 161 );
		emu_WriteWord( &bHandled, cust_addr(aud[ 0 ].ac_vol), 64 );
		emu_WriteWord( &bHandled, cust_addr(dmacon), DMAF_SETCLR | DMAF_AUD0 );
		emu_WriteWord( &bHandled, cust_addr(aud[ 0 ].ac_len), 1 );

		Delay(20);
		free(wave);
	}

}

void init_rc(struct rc *rc, struct PUHData* pd)
{
	rc -> rc = FALSE;
	rc -> quit = FALSE;
	rc -> messed_with_registers = FALSE;
	rc -> audio_mode = 0;
	rc -> frequency = 0;
	rc -> code = 0;
	rc -> pd = pd;
	rc -> AHI_name[0] = 0;
}

void about()
{
	int about_text_size = 0;
	char *about_text; 

	about_text_size = strlen(_L(str_copyright_by) )+1;
	about_text_size += strlen(_L(str_ported_by) )+1;
	about_text_size += strlen(_L(str_updated_by) )+1;
	about_text_size += strlen(_L(str_ciaa_ciab) )+1;
	about_text_size += strlen(_L(str_contributers) )+1;

	about_text = malloc( about_text_size );

	snprintf(about_text, about_text_size, "%s\n%s\n%s\n%s\n%s",
			_L(str_copyright_by),
			_L(str_ported_by),
			_L(str_updated_by),
			_L(str_ciaa_ciab),
			_L(str_contributers));

	req( _L(win_about_title), about_text,_L(req_ok), 0);

	free(about_text);
}

ULONG getv( ULONG id, ULONG arg ) 
{
	ULONG ret;

	GetAttr( arg, obj[ id ], &ret );
	return ret; 
}

void IO_BUTTONS_UP( void );
void IO_BUTTONS_DOWN(ULONG ID);

void activate( struct rc *rc )
{
	if ( ! InstallPUH( 0, rc -> audio_mode, rc -> frequency ) )
	{
		req(_L(win_sorry_title),_L(str_cant_open_ahi),_L(req_ok), 3);
	}
	else
	{
		options.installed = TRUE;

		if (  ActivatePUH( rc -> pd ) )
		{
			options.activated = TRUE;
		}
	}
}

bool deactivate( struct rc *rc )
{
	bool deactivate = false;

	if (
		(allocated_timers( &chip_ciaa ) +allocated_timers( &chip_ciab )) > 0
	)
	{
		ULONG ret;
		char *buttons = (char *) alloca( strlen(_L(req_ok)) + strlen(_L(req_ignore)) + 2 );
		sprintf(buttons,"%s|%s",_L(req_ok),_L(req_ignore));
		ret = req(_L(win_warning_title),_L(str_warn_close),buttons, 3);

		if (ret == 0) // ignore
		{
			int n;

			// stop timers!!

			init_chip_timer( &chip_ciaa.a );
			init_chip_timer( &chip_ciaa.b );
			chip_ciaa.icr = 0;

			init_chip_timer( &chip_ciab.a );
			init_chip_timer( &chip_ciab.b );
			chip_ciab.icr = 0;

			Delay(10);	// now its maybe its safe to remove IRQ's, and die!!!

			for (n=0;n<2;n++)
			{
				chip_ciaa.interrupts[n] = NULL;
				chip_ciab.interrupts[n] = NULL;
			}

			deactivate = true;
		}
	}
	else deactivate = true;

	if (deactivate)
	{
		UninstallPUH( rc -> pd );
		options.activated = FALSE;

		if (win[ win_prefs ]) update_gui( win_prefs, NULL );
	}

	return deactivate;
}


void HandleGadgetsUp(ULONG input_flags , struct rc *rc)
{
	switch( input_flags & RL_GADGETMASK )
	{
		case LIST_Frequency:
			update_hz();
			cia_frequency_select = getv( LIST_Frequency, CHOOSER_Selected );
			break;

		case LIST_BLITTER:
			blitter_selected = blitter_lib_id[getv( LIST_BLITTER, CHOOSER_Selected )];
			printf("blitter_selected: %d\n", blitter_selected );
			break;

		case GAD_SELECT_MODE:
			{
				struct AHIAudioModeRequester* req = NULL;
								
				struct TagItem	filter_tags[] =
				{
					{ AHIDB_Realtime, TRUE },
					{ AHIDB_MaxChannels, 4 },
					{ TAG_DONE, 0 }
				};

				struct Hook filter_hook =
				{
					{ NULL, NULL },
					(HOOKFUNC) FilterFunc,
					NULL,
					NULL
				};

				req = AHI_AllocAudioRequest(
						AHIR_Window, 	(ULONG) win[ win_prefs ],
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
					rc->rc = FALSE;
				}
				else
				{
					if ( AHI_AudioRequest( req, TAG_DONE ) )
					{
						rc -> audio_mode = req->ahiam_AudioID;
						rc -> frequency = req->ahiam_MixFreq;
										
						if ( AHI_GetAudioAttrs( rc -> audio_mode, NULL, 
								AHIDB_BufferLen, sizeof( rc -> AHI_name ), 
								AHIDB_Name, (ULONG) rc -> AHI_name, 
								TAG_DONE ) )
						{
							update_gui( win_prefs, rc );
						}
					}

					AHI_FreeAudioRequest( req );
				}
				break;
			}

		case GAD_ACTIVATE:
			activate( rc );
			update_gui( win_prefs, NULL );
			break;

		case GAD_DEACTIVATE:
			deactivate( rc );
			break;
				
		case GAD_TEST:
			{
				// this buttons should only enabled, when NallePuh is active.

				rc -> messed_with_registers = TRUE;			
				nallepuh_test();
			}
			break;

		case GAD_ABOUT:
			about();
			break;
	}

	 IO_BUTTONS_DOWN(input_flags & RL_GADGETMASK);
}

#define ACTIVE_HI_SET(REG,BIT) REG  |= BIT
#define ACTIVE_HI_CLEAR(REG,BIT) REG &= ~BIT 
#define ACTIVE_LOW_SET(REG,BIT) REG &= ~BIT
#define ACTIVE_LOW_CLEAR(REG,BIT) REG |= BIT 

void IO_BUTTONS_DOWN(ULONG ID)
{
	switch( ID )
	{
		case GAD_JOY1_BUTTON1:
			gettimeofday(&button_press_time[0], NULL); 
			ACTIVE_LOW_SET(ciaa_pra,PA6); 
			break;

		case GAD_JOY1_BUTTON2:
			gettimeofday(&button_press_time[1], NULL); 
			ACTIVE_LOW_SET(potgor,B10); 
			break;

		case GAD_JOY2_BUTTON1:
			gettimeofday(&button_press_time[2], NULL); 
			ACTIVE_LOW_SET(ciaa_pra,PA7); 
			break;

		case GAD_JOY2_BUTTON2:
			gettimeofday(&button_press_time[3], NULL); 
			ACTIVE_LOW_SET(potgor,B14); 
			break;
	}
}

// work around for reaction not supporting button down!!!!!!! f**k reaction!!!

bool is_button_time( struct timeval *current_time, int n, int min_usec )
{
	int usec;
	struct timeval diff ;

	timersub(current_time,  &button_press_time[n] ,&diff);
	usec = (diff.tv_sec * 1000) + diff.tv_usec ;

	return usec > min_usec;
}

void IO_BUTTONS_UP()
{
	// swap bits thats have pull up!.
	ULONG inv_potgor = potgor ^ 0x5500;
	ULONG inv_ciaa_pra = potgor ^ 0xC0;

	int timeout = 500000;

	struct timeval current_time;
	gettimeofday(&current_time, NULL); 

	if ( (inv_ciaa_pra & PA6) && (is_button_time( &current_time, 0, timeout )) )
	{
		ACTIVE_LOW_CLEAR(ciaa_pra,PA6);
	}

	if ( (inv_potgor & B10 ) && is_button_time( &current_time, 1, timeout) )
	{
		ACTIVE_LOW_CLEAR(potgor,B10);
	}

	if ( (inv_ciaa_pra & PA7) && is_button_time( &current_time, 2, timeout) )
	{
		ACTIVE_LOW_CLEAR(ciaa_pra,PA7); 
	}

	if ( (inv_potgor & B14) && is_button_time( &current_time, 3, timeout) )
	{
		ACTIVE_LOW_CLEAR(potgor,B14);
	}
}


void HandleGUI( struct Window * window, struct PUHData* pd, struct rc *rc )
{
	ULONG mask;
	ULONG window_signals = 0;

	update_gui( win_prefs, rc );

	window_signals = 1L << window -> UserPort -> mp_SigBit;

	while( ! rc -> quit )
	{
		mask = Wait( window_signals | SIGBREAKF_CTRL_C | timer_mask | refresh_signal );
		
		if ( mask & SIGBREAKF_CTRL_C )
		{
			if (deactivate( &rc ))
			{
				rc -> quit = TRUE;
				rc -> rc	= TRUE;
			}
			break;
		}

		if ( mask & refresh_signal )
		{
			if (HIT_Last_Flags != HIT_Flags)
			{
				char buffer[1000];
				snprintf(buffer,sizeof(buffer),"%s",window_title_name);
				if (HIT_Flags & HIT_CUSTOM) 	snprintf(buffer,sizeof(buffer),"%s %s", buffer, " [CUSTOM]");
				if (HIT_Flags & HIT_CIAA) 	snprintf(buffer,sizeof(buffer),"%s %s", buffer, " [CIA A]");
				if (HIT_Flags & HIT_CIAB) 	snprintf(buffer,sizeof(buffer),"%s %s", buffer, " [CIA B]");
				SetWindowTitles(window, buffer, buffer);
			}
			HIT_Last_Flags = HIT_Flags;
			HIT_Flags = 0;

			reactivate_refresh_timer();
		}

		if ( mask & window_signals )
		{
			ULONG input_flags = 0;
			
			while( ( input_flags = RA_HandleInput( layout[ win_prefs ] ,&(rc -> code)) ) != WMHI_LASTMSG )
			{
				switch( input_flags & WMHI_CLASSMASK)
				{
					case WMHI_CLOSEWINDOW:

						if (deactivate( rc ))
						{
							rc -> quit = TRUE;
							rc -> rc	= TRUE;
						}
						break;

					case WMHI_ICONIFY:

						// empty event queue
						while( ( input_flags = RA_HandleInput( layout[ win_prefs ] ,&(rc -> code) ) ) != WMHI_LASTMSG );

						close_window( win_prefs );

						handel_iconify();

						init_prefs(win_prefs);			// we trashed objs with close_window :-)
						if (open_window(win_prefs, rc ))
						{
							window = win[ win_prefs ];
							update_gui( win_prefs, rc );
						}
						else return;
						break;

					case WMHI_GADGETUP:
						HandleGadgetsUp(input_flags, rc);
						break;

					default:
						break;
				}
			}
		}
	}
	
	save("progdir:NallePuh.cfg",rc);

}

