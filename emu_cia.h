
/*
	Copyright (C) 2023: Kjetil Hvalstrand
	MIT License.
*/


struct cia_timer
{
	int cr;
	int ticks;
	int ticks_latch;
	int new_ticks;
};

struct chip
{
	int icr ;
	struct cia_timer a;
	struct cia_timer b;
	ULONG interrupt_mask;
	struct Interrupt *interrupts[2];
	ULONG signal;
	int icr_handle;			// do not clear on read, clear on interupt.
};

extern void init_chip( struct chip *chip, ULONG sig );
extern bool us2ticks( double unit, uint32 *us, uint32 *ticks );
extern void do_cia_timer_a(struct chip *chip);
extern void do_cia_timer_b(struct chip *chip);

ULONG CIAA(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd);
ULONG CIAB(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd);

#define SET 1
#define CLR 0

#define CRB5 (1<5)
#define CRB6 (1<6)

#define INMODE_A(n) ((n&(CRB5)) >> 5)
#define INMODE_B(n) ((n&(CRB5|CRB6))>>5)


enum CTRL_REG
{
	START = 0,
	PBON,
	OUTMODE,
	RUNMODE,
	LOAD,
	INMODE,
	SPMODE
};

#define SET_REG_BIT(chip_reg,bit,set)	chip_reg = set ? (chip_reg | (1<<bit)) : (chip_reg & ~(1<<bit));

// joy ports... just ignore...

#define DDRA 0x200
#define DDRB 0x300

// Timer registers.

#define TALO 0x400
#define TAHI 0x500

#define TBLO 0x600
#define TBHI 0x700

#define TODLO 0x800
#define TODMID 0x900
#define TODHI 0xA00

#define SDR 0xC00
#define ICR 0xD00
#define CRA 0xE00
#define CRB 0xF00

// hz in micro seconds.

#define MAX_SCANLINES 286

extern int HORIZONTAL_SYNC_HZ;
extern int VSYNC_HZ;

extern void update_hz();

extern double CIA_TIMER_UNIT;
