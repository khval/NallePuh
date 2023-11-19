VERSION = 1
REVISION = 6

.macro DATE
.ascii "5.11.2023"
.endm

.macro VERS
.ascii "NallePuh 1.6"
.endm

.macro VSTRING
.ascii "NallePuh 1.6 (5.11.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePuh 1.6 (5.11.2023)"
.byte 0
.endm
