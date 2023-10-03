VERSION = 1
REVISION = 2

.macro DATE
.ascii "1.10.2023"
.endm

.macro VERS
.ascii "NallePUH 1.2"
.endm

.macro VSTRING
.ascii "NallePUH 1.2 (1.10.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePUH 1.2 (1.10.2023)"
.byte 0
.endm
