VERSION = 1
REVISION = 8

.macro DATE
.ascii "8.12.2023"
.endm

.macro VERS
.ascii "NallePuh 1.8"
.endm

.macro VSTRING
.ascii "NallePuh 1.8 (8.12.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePuh 1.8 (8.12.2023)"
.byte 0
.endm
