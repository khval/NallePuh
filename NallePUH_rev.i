VERSION		EQU	1
REVISION	EQU	2

DATE	MACRO
		dc.b '1.10.2023'
		ENDM

VERS	MACRO
		dc.b 'NallePUH 1.2'
		ENDM

VSTRING	MACRO
		dc.b 'NallePUH 1.2 (1.10.2023)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: NallePUH 1.2 (1.10.2023)',0
		ENDM
