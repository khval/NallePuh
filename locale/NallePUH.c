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

#define win_title 50
#define win_about_title 51
#define win_sorry_title 52
#define win_warning_title 53
#define GAD_ACTIVATE 150
#define GAD_DEACTIVATE 151
#define GAD_MESSAGES 152
#define GAD_MODE_ID 153
#define GAD_MODE_INFO 154
#define GAD_SELECT_MODE 155
#define GAD_TEST 156
#define GAD_ABOUT 157
#define PAGES_CHIP_ID 158
#define LIST_Chips_Paula 159
#define LIST_Chips_CIA 160
#define LIST_Chips_Status 161
#define LIST_Frequency 162
#define GAD_JOY1_BUTTON1 163
#define GAD_JOY1_BUTTON2 164
#define GAD_JOY2_BUTTON1 165
#define GAD_JOY2_BUTTON2 166
#define GAD_CIAA_CR 167
#define GAD_CIAA_TA 168
#define GAD_CIAA_TB 169
#define GAD_CIAB_CR 170
#define GAD_CIAB_TA 171
#define GAD_CIAB_TB 172
#define frame_audio 300
#define frame_port1 301
#define frame_port2 302
#define str_copyright_by 350
#define str_ported_by 351
#define str_updated_by 352
#define str_ciaa_ciab 353
#define str_contributers 354
#define str_cant_open_ahi 355
#define str_warn_close 356
#define req_ok 400
#define req_ignore 401
#define req_cancel 402
#define ID_END 500

#endif /* CATCOMP_NUMBERS */


/****************************************************************************/


#ifdef CATCOMP_STRINGS

#define win_title_STR "Nalle Puh"
#define win_about_title_STR "About Nalle Puh"
#define win_sorry_title_STR "Sorry!!"
#define win_warning_title_STR "Warning!!"
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
#define LIST_Chips_Status_STR "Status"
#define LIST_Frequency_STR "Frequency"
#define GAD_JOY1_BUTTON1_STR "Button1"
#define GAD_JOY1_BUTTON2_STR "Button2"
#define GAD_JOY2_BUTTON1_STR "Button1"
#define GAD_JOY2_BUTTON2_STR "Button2"
#define GAD_CIAA_CR_STR "Controll Register"
#define GAD_CIAA_TA_STR "Timer A"
#define GAD_CIAA_TB_STR "Timer B"
#define GAD_CIAB_CR_STR "Controll Register"
#define GAD_CIAB_TA_STR "Timer A"
#define GAD_CIAB_TB_STR "Timer B"
#define frame_audio_STR "Audio options"
#define frame_port1_STR "Port1: Joystick 2 / Mouse"
#define frame_port2_STR "Port2: Joystick 1"
#define str_copyright_by_STR "(c)2001 Martin Blom <martin@blom.org>"
#define str_ported_by_STR "Ported to AmigaOS4.0 by: Stephan Rupprecht (2004)"
#define str_updated_by_STR "Updated for AmigaOS4.1 FE by: Kjetil Hvalstrand (2022)"
#define str_ciaa_ciab_STR "CIAA/CIAB support added (2023)"
#define str_contributers_STR "Contributers: Damien Stewart, Samir Hawamdeh, Javier de las Rivas"
#define str_cant_open_ahi_STR "Can't open AHI & set memory exception handler"
#define str_warn_close_STR "Several timer interrupts are reserved, please make sure all programs are properly closed before continuing, failure to do so, can result in hanging programs, or in the worst-case crashes, do you choose to ignore?"
#define req_ok_STR "_Ok"
#define req_ignore_STR "_Ignore"
#define req_cancel_STR "Cancel"
#define ID_END_STR "Size of locale"

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
    {win_sorry_title,(CONST_STRPTR)win_sorry_title_STR},
    {win_warning_title,(CONST_STRPTR)win_warning_title_STR},
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
    {LIST_Chips_Status,(CONST_STRPTR)LIST_Chips_Status_STR},
    {LIST_Frequency,(CONST_STRPTR)LIST_Frequency_STR},
    {GAD_JOY1_BUTTON1,(CONST_STRPTR)GAD_JOY1_BUTTON1_STR},
    {GAD_JOY1_BUTTON2,(CONST_STRPTR)GAD_JOY1_BUTTON2_STR},
    {GAD_JOY2_BUTTON1,(CONST_STRPTR)GAD_JOY2_BUTTON1_STR},
    {GAD_JOY2_BUTTON2,(CONST_STRPTR)GAD_JOY2_BUTTON2_STR},
    {GAD_CIAA_CR,(CONST_STRPTR)GAD_CIAA_CR_STR},
    {GAD_CIAA_TA,(CONST_STRPTR)GAD_CIAA_TA_STR},
    {GAD_CIAA_TB,(CONST_STRPTR)GAD_CIAA_TB_STR},
    {GAD_CIAB_CR,(CONST_STRPTR)GAD_CIAB_CR_STR},
    {GAD_CIAB_TA,(CONST_STRPTR)GAD_CIAB_TA_STR},
    {GAD_CIAB_TB,(CONST_STRPTR)GAD_CIAB_TB_STR},
    {frame_audio,(CONST_STRPTR)frame_audio_STR},
    {frame_port1,(CONST_STRPTR)frame_port1_STR},
    {frame_port2,(CONST_STRPTR)frame_port2_STR},
    {str_copyright_by,(CONST_STRPTR)str_copyright_by_STR},
    {str_ported_by,(CONST_STRPTR)str_ported_by_STR},
    {str_updated_by,(CONST_STRPTR)str_updated_by_STR},
    {str_ciaa_ciab,(CONST_STRPTR)str_ciaa_ciab_STR},
    {str_contributers,(CONST_STRPTR)str_contributers_STR},
    {str_cant_open_ahi,(CONST_STRPTR)str_cant_open_ahi_STR},
    {str_warn_close,(CONST_STRPTR)str_warn_close_STR},
    {req_ok,(CONST_STRPTR)req_ok_STR},
    {req_ignore,(CONST_STRPTR)req_ignore_STR},
    {req_cancel,(CONST_STRPTR)req_cancel_STR},
    {ID_END,(CONST_STRPTR)ID_END_STR},
};

#endif /* CATCOMP_ARRAY */


/****************************************************************************/



#endif /* NALLEPUH_H */
