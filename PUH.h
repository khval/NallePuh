/* $Id: PUH.h,v 1.9 2001/04/28 17:48:33 lcs Exp $ */

/*
     NallePUH -- Paula utan henne -- A minimal Paula emulator.
     Copyright (C) 2001 Martin Blom <martin@blom.org>
     
     This program is free software; you can redistribute it and/or
     modify it under the terms of the GNU General Public License
     as published by the Free Software Foundation; either version 2
     of the License, or (at your option) any later version.
     
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
     
     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef NallePUH_PUH_h
#define NallePUH_PUH_h

#if USE_DEBUG
#define DEBUG(...)	if (debug) DebugPrintF(__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#ifndef __amigaos4__
#include "CompilerSpecific.h"
#endif

#include <exec/interrupts.h>
#include <utility/hooks.h>

struct ExceptionData;
struct ExecBase;
struct Library;
struct MMUContext;

/* Debugging */

#ifdef __amigaos4__
#define KPrintFArgs	DebugPrintF
#else
void
KPrintFArgs( UBYTE* fmt,
             LONG*  args );


#define KPrintF( fmt, ... )        \
({                                 \
  LONG _args[] = { __VA_ARGS__ };  \
  KPrintFArgs( (fmt), _args );     \
})
#endif


/* Make nice accesses to hardware registers */

/*
UWORD ReadWord( void* address );
void WriteWord(  void* address, UWORD value );
ULONG ReadLong( void* address );
void WriteLong(  void* address, ULONG value );
*/

void emu_WriteWord( BOOL *bHandled, void *address, UWORD value );
void emu_WriteLong( BOOL *bHandled, void *address, ULONG value );

/* The emulator */


/* Flags for InstallPUH() and PUHData.m_Flags */

#define PUHF_NONE       0L
#define PUHF_PATCH_ROM	(1L << 0)
#define PUHF_PATCH_APPS	(1L << 1)
#define PUHF_TOGGLE_LED (1L << 2)

#define PUHB_PATCH_ROM	0
#define PUHB_PATCH_APPS	1
#define PUHB_TOGGLE_LED 2

enum
{
	HIT_NONE,
	HIT_CUSTOM = 1,
	HIT_CIAA = 2,
	HIT_CIAB = 4
};

extern uint32 HIT_Flags, HIT_Last_Flags;

struct paula_channel
{
	ULONG		id;
	BOOL		SoundOn;
	BOOL		GotDatLo;
	BOOL		GotDatHi;
	ULONG		SoundLocationLoad;			// parts are latched in...
 	ULONG		SoundLocationCurrent;		// played off this...
	ULONG		SoundLength;
};

struct PUHData
{
	BOOL                  m_Active;
	UWORD                 m_Pad;

	ULONG			m_Flags;

	struct Hook*          m_LogHook;

	ULONG                 m_AudioMode;
	struct AHIAudioCtrl*  m_AudioCtrl;

	struct Hook           m_SoundFunc;

	ULONG                 m_ChipFreq;
     
	UWORD                 m_DMACON;
	UWORD                 m_INTREQ;
	UWORD                 m_INTENA;
	BOOL                  m_CausePending;

	struct paula_channel channels[4];

	void*                 m_Intercepted;
	void*                 m_CustomDirect;
	ULONG	               m_CustomSize;

	struct Interrupt     m_SoftInt;
	struct Interrupt	  m_FaultInt;
	struct Interrupt *	  m_OldFaultInt;

};

struct options
{
	BOOL installed;
	BOOL activated;
} ;

struct PUHData *AllocPUH( void );
void FreePUH( struct PUHData* pd );
void SetPUHLogger( struct Hook*    hook,    struct PUHData* pd );
void VARARGS68K LogPUH( struct PUHData* pd, CONST_STRPTR fmt,   ... );
BOOL InstallPUH( ULONG  flags, ULONG audio_mode, ULONG frequency );
void UninstallPUH( struct PUHData* pd );
BOOL ActivatePUH( struct PUHData* pd );
BOOL DeactivatePUH( struct PUHData* pd );

#endif /* NallePUH_PUH_h */
