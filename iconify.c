
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/gadtools.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <workbench/startup.h>

struct MsgPort *iconifyPort = NULL;
struct DiskObject *dobj = NULL;
struct AppIcon *appicon;
ULONG iconify_sig;

/*
void save_window_attr( struct window *win)
{
	GetWindowAttr( win,  WA_Left, &self -> window_left, sizeof(int *));
	GetWindowAttr( win,  WA_Top, &self -> window_top, sizeof(int *));
	GetWindowAttr( win,  WA_InnerWidth, &self -> window_width, sizeof(int *));
	GetWindowAttr( win,  WA_InnerHeight, &self -> window_height, sizeof(int *));
}
*/


BOOL enable_Iconify( )
{
	dobj = GetDiskObject( "progdir:nallepuh" );
	if (dobj)
	{
		iconifyPort = (struct MsgPort *) AllocSysObject(ASOT_PORT,NULL);

		if (iconifyPort)
		{
			appicon = AddAppIcon(1, 0, "Amos Kittens", iconifyPort, 0, dobj, WBAPPICONA_SupportsOpen, TRUE,TAG_END);
			if (appicon) return TRUE;
		}
	}

	return FALSE;
}

void dispose_Iconify()
{
	if (appicon)
	{
		RemoveAppIcon( appicon );
		appicon = NULL;
	}

	if (dobj)
	{
		FreeDiskObject(dobj);
		dobj = NULL;
	}

	if (iconifyPort)
	{
		FreeSysObject ( ASOT_PORT, iconifyPort ); 
		iconifyPort = NULL;
		iconify_sig  = 0;
	}
}

void	disable_Iconify()
{
	dispose_Iconify();
}


