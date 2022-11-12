#ifndef NALLEPUH_H
#define NALLEPUH_H


/****************************************************************************/


/* This file was created automatically by CatComp.
 * Do NOT edit by hand!
 */


#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifdef CATCOMP_CODE
#ifndef CATCOMP_BLOCK
#define CATCOMP_ARRAY
#endif
#endif

#ifdef CATCOMP_ARRAY
#ifndef CATCOMP_NUMBERS
#define CATCOMP_NUMBERS
#endif
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif

#ifdef CATCOMP_BLOCK
#ifndef CATCOMP_STRINGS
#define CATCOMP_STRINGS
#endif
#endif


/****************************************************************************/


#ifdef CATCOMP_NUMBERS

#define GAD_ACTIVATE 1
#define GAD_DEACTIVATE 2
#define GAD_INSTALL 3
#define GAD_MESSAGES 4
#define GAD_MODE_INFO 5
#define GAD_MODE_SELECT 6
#define GAD_PATCH_APPS 7
#define GAD_PATCH_ROM 8
#define GAD_TEST 9
#define GAD_TOGGLE_LED 10
#define GAD_UNINSTALL 11
#define msgPatchROM 100
#define msgPatchApplications 101
#define msgAudioMode 102
#define msgInstall 103
#define msgUninstall 104
#define msgActivate 105
#define msgDeactivate 106
#define msgNallePUH 107
#define msgTest 108
#define msgToggleLed 109
#define msgActivityLog 110

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define GAD_ACTIVATE_STR "Activate"
#define GAD_DEACTIVATE_STR "Deactivate"
#define GAD_INSTALL_STR "Install"
#define GAD_MESSAGES_STR "Messages"
#define GAD_MODE_INFO_STR "Mode info"
#define GAD_MODE_SELECT_STR "Mode select"
#define GAD_PATCH_APPS_STR "Patch apps"
#define GAD_PATCH_ROM_STR "Patch rom"
#define GAD_TEST_STR "Test"
#define GAD_TOGGLE_LED_STR "Toggele led"
#define GAD_UNINSTALL_STR "Uninstall"
#define msgPatchROM_STR "Patch _ROM"
#define msgPatchApplications_STR "Patch a_pplications"
#define msgAudioMode_STR "Audio _mode..."
#define msgInstall_STR "_Install"
#define msgUninstall_STR "_Uninstall"
#define msgActivate_STR "_Activate"
#define msgDeactivate_STR "_Deactivate"
#define msgNallePUH_STR "Nalle PUH version 0.3 (c)2001 Martin Blom <martin@blom.org>"
#define msgTest_STR "_Test"
#define msgToggleLed_STR "Toggle _led on exceptions"
#define msgActivityLog_STR "[Activity log area]"

#endif /* CATCOMP_STRINGS */


/****************************************************************************/


#ifdef CATCOMP_ARRAY

struct CatCompArrayType
{
    LONG         cca_ID;
    CONST_STRPTR cca_Str;
};

STATIC CONST struct CatCompArrayType CatCompArray[] =
{
    {GAD_ACTIVATE,(CONST_STRPTR)GAD_ACTIVATE_STR},
    {GAD_DEACTIVATE,(CONST_STRPTR)GAD_DEACTIVATE_STR},
    {GAD_INSTALL,(CONST_STRPTR)GAD_INSTALL_STR},
    {GAD_MESSAGES,(CONST_STRPTR)GAD_MESSAGES_STR},
    {GAD_MODE_INFO,(CONST_STRPTR)GAD_MODE_INFO_STR},
    {GAD_MODE_SELECT,(CONST_STRPTR)GAD_MODE_SELECT_STR},
    {GAD_PATCH_APPS,(CONST_STRPTR)GAD_PATCH_APPS_STR},
    {GAD_PATCH_ROM,(CONST_STRPTR)GAD_PATCH_ROM_STR},
    {GAD_TEST,(CONST_STRPTR)GAD_TEST_STR},
    {GAD_TOGGLE_LED,(CONST_STRPTR)GAD_TOGGLE_LED_STR},
    {GAD_UNINSTALL,(CONST_STRPTR)GAD_UNINSTALL_STR},
    {msgPatchROM,(CONST_STRPTR)msgPatchROM_STR},
    {msgPatchApplications,(CONST_STRPTR)msgPatchApplications_STR},
    {msgAudioMode,(CONST_STRPTR)msgAudioMode_STR},
    {msgInstall,(CONST_STRPTR)msgInstall_STR},
    {msgUninstall,(CONST_STRPTR)msgUninstall_STR},
    {msgActivate,(CONST_STRPTR)msgActivate_STR},
    {msgDeactivate,(CONST_STRPTR)msgDeactivate_STR},
    {msgNallePUH,(CONST_STRPTR)msgNallePUH_STR},
    {msgTest,(CONST_STRPTR)msgTest_STR},
    {msgToggleLed,(CONST_STRPTR)msgToggleLed_STR},
    {msgActivityLog,(CONST_STRPTR)msgActivityLog_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/



#endif /* NALLEPUH_H */
