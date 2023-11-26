VERSION = 1
REVISION = 7

.macro DATE
.ascii "26.11.2023"
.endm

.macro VERS
.ascii "NallePuh 1.7"
.endm

.macro VSTRING
.ascii "NallePuh 1.7 (26.11.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePuh 1.7 (26.11.2023)"
.byte 0
.endm
