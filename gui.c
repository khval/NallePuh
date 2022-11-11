
#include <classes/window.h>
#include <gadgets/listbrowser.h>
#include <intuition/gadgetclass.h>
#include <libraries/resource.h>
#include <proto/listbrowser.h>
#include <proto/resource.h>

/******************************************************************************
** GUI utility functions ******************************************************
******************************************************************************/

ULONG RefreshSetGadgetAttrsA( struct Gadget *	g, struct Window *w, struct Requester* r, struct TagItem* tags )
{
	ULONG retval;
	BOOL	changedisabled = FALSE;
	BOOL	disabled	= FALSE;

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

struct Node*LBAddNodeA( struct Gadget *	lb, struct Window *w, struct Requester *r, struct Node *n, struct TagItem* tags )
{
	return (struct Node*) DoGadgetMethod( lb, w, r,LBM_ADDNODE, NULL, (ULONG) n, (ULONG) tags );
}

struct Node *LBAddNode( struct Gadget *lb, struct Window *w, struct Requester* r, struct Node *n, Tag tag1, ... )
{
	return (struct Node*) DoGadgetMethod( lb, w, r, LBM_ADDNODE, NULL, (ULONG) n, (ULONG) &tag1 );
}

/******************************************************************************
** ShowGUI ********************************************************************
******************************************************************************/

static BOOL ShowGUI( struct PUHData* pd )
{
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
								WINDOW_AppPort, 	(ULONG) app_port,
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

ASMCALL SAVEDS static ULONG FilterFunc( REG( a0, struct Hook *hook ), REG( a2, struct AHIAudioModeRequester* req ), REG( a1, ULONG mode_id ) )
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
						LBNA_Column, 	0,
						LBNCA_Text, 	(ULONG) message,
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

unsigned int pooh11_sblen;

static BOOL HandleGUI( Object * window, struct Gadget** gadgets, struct PUHData* pd )
{
	BOOL	rc = FALSE;
	BOOL	quit = FALSE;
	BOOL	messed_with_registers = FALSE;

	struct Window* win_ptr = NULL;
	ULONG	window_signals = 0;

	ULONG	audio_mode = 0;
	ULONG	frequency = 0;

	struct Custom *custom = (struct Custom*) 0xdff000;
	struct LogData log_data;

	struct Hook log_hook =
	{
		{ NULL, NULL },
		(HOOKFUNC) LogToList,
		NULL,
		&log_data
	};

	CopyMem( pooh11_sb, chip, pooh11_sblen );

	GetAttr( WINDOW_SigMask, window, &window_signals );
	GetAttr( WINDOW_Window, window, (ULONG*) &win_ptr );

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
			rc	= TRUE;
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
						rc	= TRUE;
						break;

					case WMHI_ICONIFY:
						DoMethod( window, WM_ICONIFY );
						GetAttr( WINDOW_Window, window, (ULONG*) &win_ptr );
						log_data.m_Window = win_ptr;
						break;
						
					case WMHI_UNICONIFY:
						DoMethod( window, WM_OPEN );
						GetAttr( WINDOW_Window, window, (ULONG*) &win_ptr );
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
										AHIR_Window, 	(ULONG) win_ptr,
										AHIR_SleepWindow, TRUE,
										AHIR_InitialAudioID, audio_mode,
										AHIR_InitialMixFreq, frequency,
										AHIR_DoMixFreq, TRUE,
										AHIR_DoDefaultMode, TRUE,
										AHIR_FilterFunc, (ULONG) &filter_hook,
										AHIR_FilterTags, (ULONG) filter_tags,
										TAG_DONE );

								if ( req == NULL )
								{
									quit = TRUE;
									rc	= FALSE;
								}
								else
								{
									if ( AHI_AudioRequest( req, TAG_DONE ) )
									{
										char buffer[ 256 ];

										audio_mode = req->ahiam_AudioID;
										frequency = req->ahiam_MixFreq;
										
										if ( AHI_GetAudioAttrs( audio_mode, NULL, AHIDB_BufferLen, 255, AHIDB_Name, (ULONG) buffer, TAG_DONE ) )
										{
											RefreshSetGadgetAttrs( gadgets[ GAD_MODE_INFO ], win_ptr, NULL,
													STRINGA_TextVal, (ULONG) buffer, TAG_DONE );

											RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
													GA_Disabled, FALSE, TAG_DONE );
										}
									}

									AHI_FreeAudioRequest( req );
								}

								break;
							}

							case GAD_INSTALL:
							{
								unsigned int flags = 0;
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

								if ( ! InstallPUH( flags, audio_mode, frequency, pd ) )
								{
									LogPUH( pd, "Unable to install PUH." );
								}
								else
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_ROM ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_TOGGLE_LED ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_MODE_SELECT ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_UNINSTALL ], win_ptr, NULL,
											GA_Disabled, FALSE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
											GA_Disabled, FALSE, TAG_DONE );

									RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
											GA_Disabled, TRUE, TAG_DONE );
								}

								break;
							}


							case GAD_UNINSTALL:
							{
								ULONG patch_rom = 0;

								ClearList( &log_data );

								UninstallPUH( pd );

								RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_ROM ], win_ptr, NULL,
										GA_Disabled, FALSE, TAG_DONE );

								GetAttr( GA_Selected, gadgets[ GAD_PATCH_ROM	], &patch_rom );
								
								if ( patch_rom )
								{
									RefreshSetGadgetAttrs( gadgets[ GAD_PATCH_APPS ], win_ptr, NULL,
											GA_Disabled, FALSE, TAG_DONE );
								}

								RefreshSetGadgetAttrs( gadgets[ GAD_TOGGLE_LED ], win_ptr, NULL,
										GA_Disabled, FALSE, TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_MODE_SELECT ], win_ptr, NULL,
										GA_Disabled, FALSE, TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_INSTALL ], win_ptr, NULL,
										GA_Disabled, FALSE, TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_UNINSTALL ], win_ptr, NULL,
										GA_Disabled, TRUE, TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_ACTIVATE ], win_ptr, NULL,
										GA_Disabled, TRUE, TAG_DONE );

								RefreshSetGadgetAttrs( gadgets[ GAD_DEACTIVATE ], win_ptr, NULL,
										GA_Disabled, TRUE, TAG_DONE );
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
