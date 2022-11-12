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



#endif /* NALLEPUH_H */
