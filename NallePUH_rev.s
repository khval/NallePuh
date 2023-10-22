VERSION = 1
REVISION = 4

.macro DATE
.ascii "22.10.2023"
.endm

.macro VERS
.ascii "NallePuh 1.4"
.endm

.macro VSTRING
.ascii "NallePuh 1.4 (22.10.2023)"
.byte 13,10,0
.endm

.macro VERSTAG
.byte 0
.ascii "$VER: NallePuh 1.4 (22.10.2023)"
.byte 0
.endm
