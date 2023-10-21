
/*
	This file is Copyright (C) 2023: Kjetil Hvalstrand
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
		// chip registers.

	int icr ;

		// timers.

	struct cia_timer a;
	struct cia_timer b;

		// not sure help!!!!

	ULONG interrupt_mask;

		// over flow exception set by cia*.resource

	struct Interrupt *interrupts[2];

		// chip needs to wake up, and handle expcetions.

	ULONG signal;

		// do not clear on read, clear on interrupt, so interrupt is garanied.
		// does not require read operation to set sate, so should be atomic.

	char icr_handle[8];	

		// Exec pseudo-priority irq, not hw irq.

	ULONG irq;

	const char *name;
};

extern void init_chip( const char *name, struct chip *chip, ULONG sig, ULONG irq );
extern bool us2ticks( double unit, uint32 *us, uint32 *ticks );
extern void do_cia_timer_a(struct chip *chip);
extern void do_cia_timer_b(struct chip *chip);
extern void dump_chip_interrupts( struct chip *chip );

ULONG CIAA(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd);
ULONG CIAB(struct ExceptionContext *pContext, struct ExecBase *pSysBase, struct PUHData *pd);

#define SET 1
#define CLR 0

#define PA6 (1<<6)
#define PA7 (1<<7)

#define CRB5 (1<<5)
#define CRB6 (1<<6)

#define B10 (1<<10)
#define B14 (1<<14)

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

#define PRA 0x000

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

extern ULONG potgor;
extern ULONG ciaa_pra;


