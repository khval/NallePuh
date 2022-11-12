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
#define msgPatchROM 12
#define msgPatchApplications 13
#define msgAudioMode 14
#define msgInstall 15
#define msgUninstall 16
#define msgActivate 17
#define msgDeactivate 18
#define msgNallePUH 19
#define msgTest 20
#define msgToggleLed 21
#define msgActivityLog 22
#define ID_END 23

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/



#endif /* NALLEPUH_H */
