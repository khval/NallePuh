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
#define GAD_MESSAGES 3
#define GAD_MODE_ID 4
#define GAD_MODE_INFO 5
#define GAD_SELECT_MODE 6
#define GAD_TEST 7
#define GAD_ABOUT 8
#define msgNallePUH 9
#define ID_END 10

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/



#endif /* NALLEPUH_H */
