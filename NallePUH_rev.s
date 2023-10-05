VERSION = 1
REVISION = 3

.macro DATE
.ascii "4.10.2023"
.endm

.macro VERS
.ascii "NallePUH 1.3"
.endm

.macro VSTRING
.ascii "NallePUH 1.3 (4.10.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePUH 1.3 (4.10.2023)"
.byte 0
.endm
