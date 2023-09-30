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

#define win_title 100
#define win_about_title 101
#define GAD_ACTIVATE 200
#define GAD_DEACTIVATE 201
#define GAD_MESSAGES 202
#define GAD_MODE_ID 203
#define GAD_MODE_INFO 204
#define GAD_SELECT_MODE 205
#define GAD_TEST 206
#define GAD_ABOUT 207
#define PAGES_CHIP_ID 208
#define LIST_Chips_Paula 209
#define LIST_Chips_CIA 210
#define LIST_Frequency 211
#define str_copyright_by 300
#define str_ported_by 301
#define str_updated_by 302
#define str_ciaa_ciab 303
#define str_contributers 304
#define req_ok 400
#define ID_END 401

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define win_title_STR "Nalle Puh"
#define win_about_title_STR "About Nalle Puh"
#define GAD_ACTIVATE_STR "_Activate"
#define GAD_DEACTIVATE_STR "_Deactivate"
#define GAD_MESSAGES_STR "Messages"
#define GAD_MODE_ID_STR "Mode Id"
#define GAD_MODE_INFO_STR "Mode info"
#define GAD_SELECT_MODE_STR "_Select audio mode"
#define GAD_TEST_STR "_Test"
#define GAD_ABOUT_STR "A_bout"
#define PAGES_CHIP_ID_STR "Chips"
#define LIST_Chips_Paula_STR "Paula"
#define LIST_Chips_CIA_STR "CIA"
#define LIST_Frequency_STR "Frequency"
#define str_copyright_by_STR "(c)2001 Martin Blom <martin@blom.org>"
#define str_ported_by_STR "Ported to AmigaOS4.0 by: Stephan Rupprecht (2004)"
#define str_updated_by_STR "Updated for AmigaOS4.1 FE by: Kjetil Hvalstrand (2022)"
#define str_ciaa_ciab_STR "CIAA/CIAB support added (2023)"
#define str_contributers_STR "Contributers: Damien Stewart, Samir Hawamdeh"
#define req_ok_STR "_Ok"
#define ID_END_STR ";"

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
    {win_title,(CONST_STRPTR)win_title_STR},
    {win_about_title,(CONST_STRPTR)win_about_title_STR},
    {GAD_ACTIVATE,(CONST_STRPTR)GAD_ACTIVATE_STR},
    {GAD_DEACTIVATE,(CONST_STRPTR)GAD_DEACTIVATE_STR},
    {GAD_MESSAGES,(CONST_STRPTR)GAD_MESSAGES_STR},
    {GAD_MODE_ID,(CONST_STRPTR)GAD_MODE_ID_STR},
    {GAD_MODE_INFO,(CONST_STRPTR)GAD_MODE_INFO_STR},
    {GAD_SELECT_MODE,(CONST_STRPTR)GAD_SELECT_MODE_STR},
    {GAD_TEST,(CONST_STRPTR)GAD_TEST_STR},
    {GAD_ABOUT,(CONST_STRPTR)GAD_ABOUT_STR},
    {PAGES_CHIP_ID,(CONST_STRPTR)PAGES_CHIP_ID_STR},
    {LIST_Chips_Paula,(CONST_STRPTR)LIST_Chips_Paula_STR},
    {LIST_Chips_CIA,(CONST_STRPTR)LIST_Chips_CIA_STR},
    {LIST_Frequency,(CONST_STRPTR)LIST_Frequency_STR},
    {str_copyright_by,(CONST_STRPTR)str_copyright_by_STR},
    {str_ported_by,(CONST_STRPTR)str_ported_by_STR},
    {str_updated_by,(CONST_STRPTR)str_updated_by_STR},
    {str_ciaa_ciab,(CONST_STRPTR)str_ciaa_ciab_STR},
    {str_contributers,(CONST_STRPTR)str_contributers_STR},
    {req_ok,(CONST_STRPTR)req_ok_STR},
    {ID_END,(CONST_STRPTR)ID_END_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/



#endif /* NALLEPUH_H */
