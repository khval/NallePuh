VERSION = 1
REVISION = 1

.macro DATE
.ascii "19.9.2023"
.endm

.macro VERS
.ascii "NallePUH 1.1"
.endm

.macro VSTRING
.ascii "NallePUH 1.1 (19.9.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePUH 1.1 (19.9.2023)"
.byte 0
.endm
