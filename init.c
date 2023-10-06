
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

#include "locale/NallePUH.h"
#include "gui.h"

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

struct Library *ciaaBase = NULL;
struct Library *ciabBase = NULL;

struct ciaaIFace *Iciaa = NULL;
struct ciabIFace *Iciab = NULL;

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

struct Process *MainTask = NULL;

extern void open_timer( void );
extern void close_timer( void );

extern uint32 appID;
extern struct chip chip_ciaa ;
extern struct chip chip_ciab ;

int ciaa_signal = -1;
int ciab_signal = -1;

#ifdef __amigaos4__

	#define GETIFACE(x)	if (x ## Base)  { I ## x = (struct x ## IFace *) GetInterface((struct Library *) x ## Base, "main", 1L, NULL); }
	#define DROPIFACE(x)	if (I ## x) { DropInterface((struct Interface *) I ## x); I ## x = NULL; }

	#define CloseLib( name ) \
		if (  name ## Base) CloseLibrary( name ## Base);  name ## Base = 0; \
		if (I ## name) DropInterface((struct Interface*) I ## name ); I ## name = 0;

#else

	#define GETIFACE(x)
	#define DROPIFACE(x)
	#define CloseLib( name ) if (  name ## Base) CloseLibrary( name ## Base);  name ## Base = 0; 

#endif

void CloseLibs( void );

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

extern void warn();

BOOL OpenLibs( void )
{
	// optonal

	LibBlitterBase = (struct Library *) OpenLibrary( "libblitter.library", 1 );
	ciaaBase = (struct Library *) OpenLibrary( "ciaa.resource", 1 );
	ciabBase = (struct Library *) OpenLibrary( "ciab.resource", 1 );

	GETIFACE(ciaa);
	GETIFACE(ciab);
	GETIFACE(LibBlitter);

	// Needed libs

	if ( ! open_lib( "intuition.library", 39, "main", 1, &IntuitionBase, (struct Interface **) &IIntuition) ) return FALSE;
	if ( ! open_lib( "locale.library", 39, "main", 1, &LocaleBase, (struct Interface **) &ILocale) ) return FALSE;
	if ( ! open_lib( "utility.library", 39, "main", 1, &UtilityBase, (struct Interface **) &IUtility) ) return FALSE;

	// Classes

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

	_locale = (struct Locale *) OpenLocale(NULL);

	if (_locale)
	{
		codeset_page = (ULONG *) ObtainCharsetInfo(DFCS_NUMBER, (ULONG) _locale -> loc_CodeSet , DFCS_MAPTABLE);
	}

	catalog = OpenCatalog(NULL, "NallePUH.catalog", OC_BuiltInLanguage, "english", TAG_DONE);
	if (catalog)	 _L = _L_catalog;

	appID = FindApplication(FINDAPP_Name, "NallePuh", TAG_END);
	if (appID != 0)
	{
		appID = 0;	// we did not create this appid.
		return FALSE;	// if already started.
	}

	if ( !( appID = RegisterApplication("NallePuh",REGAPP_LoadPrefs, FALSE,TAG_DONE)) ) return FALSE;

	MainTask = FindTask( NULL );
	ciaa_signal = AllocSignal(-1);
	ciab_signal = AllocSignal(-1);

	init_chip( &chip_ciaa, ciaa_signal, 4 ); 	// hw irq 2, sw irq 4 
	init_chip( &chip_ciab, ciab_signal, 14 );	// hw irq 6, sw irq 14

	open_timer();

	warn();

	return TRUE;
}


/******************************************************************************
** CloseLibs ******************************************************************
******************************************************************************/

static void CloseClasses( void )
{
	CloseLib(String);
	CloseLib(Layout);
	CloseLib(Label);
	CloseLib(Chooser);
	CloseLib(Integer);
	CloseLib(ListBrowser);
	CloseLib(ClickTab);
	CloseLib(Window);
	CloseLib(ClickTab);
	CloseLib(CheckBox);
	CloseLib(Requester);
	CloseLib(Diskfont);
	CloseLib(Icon); 
	CloseLib(Workbench); 
}

void CloseLibs( void )
{
	close_timer();

	if (ciaa_signal > -1) FreeSignal(ciaa_signal);
	if (ciab_signal > -1) FreeSignal(ciab_signal);

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

	CloseLib(ciaa);
	CloseLib(ciab);

	CloseLib(Application);
	CloseLib(Intuition);
	CloseLib(Utility);
	CloseLib(Locale);
}

