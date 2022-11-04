	IFND NALLEPUHSTR_I
NALLEPUHSTR_I	SET	1


;-----------------------------------------------------------------------------


* This file was created automatically by CatComp.
* Do NOT edit by hand!
*


	IFND EXEC_TYPES_I
	INCLUDE 'exec/types.i'
	ENDC

	IFD CATCOMP_ARRAY
CATCOMP_NUMBERS SET 1
CATCOMP_STRINGS SET 1
	ENDC

	IFD CATCOMP_CODE
CATCOMP_BLOCK SET 1
	ENDC


;-----------------------------------------------------------------------------


	IFD CATCOMP_NUMBERS

	XDEF msgPatchROM
msgPatchROM EQU 256
	XDEF msgPatchApplications
msgPatchApplications EQU 257
	XDEF msgAudioMode
msgAudioMode EQU 258
	XDEF msgInstall
msgInstall EQU 259
	XDEF msgUninstall
msgUninstall EQU 260
	XDEF msgActivate
msgActivate EQU 261
	XDEF msgDeactivate
msgDeactivate EQU 262
	XDEF msgNallePUH
msgNallePUH EQU 263
	XDEF msgTest
msgTest EQU 264
	XDEF msgToggleLed
msgToggleLed EQU 265
	XDEF msgActivityLog
msgActivityLog EQU 266

	ENDC ; CATCOMP_NUMBERS


;-----------------------------------------------------------------------------


	IFD CATCOMP_STRINGS

	XDEF msgPatchROM_STR
msgPatchROM_STR: DC.B 'Patch _ROM',$00
	XDEF msgPatchApplications_STR
msgPatchApplications_STR: DC.B 'Patch a_pplications',$00
	XDEF msgAudioMode_STR
msgAudioMode_STR: DC.B 'Audio _mode...',$00
	XDEF msgInstall_STR
msgInstall_STR: DC.B '_Install',$00
	XDEF msgUninstall_STR
msgUninstall_STR: DC.B '_Uninstall',$00
	XDEF msgActivate_STR
msgActivate_STR: DC.B '_Activate',$00
	XDEF msgDeactivate_STR
msgDeactivate_STR: DC.B '_Deactivate',$00
	XDEF msgNallePUH_STR
msgNallePUH_STR: DC.B 'Nalle PUH version 0.3 ©2001 Martin Blom <martin@blom.org>',$00
	XDEF msgTest_STR
msgTest_STR: DC.B '_Test',$00
	XDEF msgToggleLed_STR
msgToggleLed_STR: DC.B 'Toggle _led on exceptions',$00
	XDEF msgActivityLog_STR
msgActivityLog_STR: DC.B '[Activity log area]',$00

	ENDC ; CATCOMP_STRINGS


;-----------------------------------------------------------------------------


	IFD CATCOMP_ARRAY

   STRUCTURE CatCompArrayType,0
	LONG cca_ID
	APTR cca_Str
   LABEL CatCompArrayType_SIZEOF

	CNOP 0,4

	XDEF CatCompArray
CatCompArray:
	XDEF _CatCompArray
_CatCompArray:
AS0:	DC.L msgPatchROM,msgPatchROM_STR
AS1:	DC.L msgPatchApplications,msgPatchApplications_STR
AS2:	DC.L msgAudioMode,msgAudioMode_STR
AS3:	DC.L msgInstall,msgInstall_STR
AS4:	DC.L msgUninstall,msgUninstall_STR
AS5:	DC.L msgActivate,msgActivate_STR
AS6:	DC.L msgDeactivate,msgDeactivate_STR
AS7:	DC.L msgNallePUH,msgNallePUH_STR
AS8:	DC.L msgTest,msgTest_STR
AS9:	DC.L msgToggleLed,msgToggleLed_STR
AS10:	DC.L msgActivityLog,msgActivityLog_STR

	ENDC ; CATCOMP_ARRAY


;-----------------------------------------------------------------------------


	IFD CATCOMP_BLOCK

	XDEF CatCompBlock
CatCompBlock:
	XDEF _CatCompBlock
_CatCompBlock:
	DC.L $100
	DC.W $C
	DC.B 'Patch _ROM',$00,$00
	DC.L $101
	DC.W $14
	DC.B 'Patch a_pplications',$00
	DC.L $102
	DC.W $10
	DC.B 'Audio _mode...',$00,$00
	DC.L $103
	DC.W $A
	DC.B '_Install',$00,$00
	DC.L $104
	DC.W $C
	DC.B '_Uninstall',$00,$00
	DC.L $105
	DC.W $A
	DC.B '_Activate',$00
	DC.L $106
	DC.W $C
	DC.B '_Deactivate',$00
	DC.L $107
	DC.W $3A
	DC.B 'Nalle PUH version 0.3 ©2001 Martin Blom <martin@blom.org>',$00
	DC.L $108
	DC.W $6
	DC.B '_Test',$00
	DC.L $109
	DC.W $1A
	DC.B 'Toggle _led on exceptions',$00
	DC.L $10A
	DC.W $14
	DC.B '[Activity log area]',$00

	ENDC ; CATCOMP_BLOCK


;-----------------------------------------------------------------------------


   STRUCTURE LocaleInfo,0
	APTR li_LocaleBase
	APTR li_Catalog
   LABEL LocaleInfo_SIZEOF

	IFD CATCOMP_CODE

	XREF      _LVOGetCatalogStr
	XDEF      _GetString
	XDEF      GetString
GetString:
_GetString:
	lea       CatCompBlock(pc),a1
	bra.s     2$
1$: move.w  (a1)+,d1
	add.w     d1,a1
2$: cmp.l   (a1)+,d0
	bne.s     1$
	addq.l    #2,a1
	move.l    (a0)+,d1
	bne.s     3$
	move.l    a1,d0
	rts
3$: move.l  a6,-(sp)
	move.l    d1,a6
	move.l    (a0),a0
	jsr       _LVOGetCatalogStr(a6)
	move.l    (sp)+,a6
	rts
	END

	ENDC ; CATCOMP_CODE


;-----------------------------------------------------------------------------


	ENDC ; NALLEPUHSTR_I
