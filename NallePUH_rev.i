VERSION		EQU	1
REVISION	EQU	1

DATE	MACRO
		dc.b '19.9.2023'
		ENDM

VERS	MACRO
		dc.b 'NallePUH 1.1'
		ENDM

VSTRING	MACRO
		dc.b 'NallePUH 1.1 (19.9.2023)',13,10,0
		ENDM

VERSTAG	MACRO
		dc.b 0,'$VER: NallePUH 1.1 (19.9.2023)',0
		ENDM