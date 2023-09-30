
CC			= ppc-amigaos-gcc
LD			= ppc-amigaos-ld
STRIP		= ppc-amigaos-strip
CATCOMP		= catcomp

USE_DEBUG=0

CFLAGS	= -fomit-frame-pointer -O2 -W -Wall \
			-Wno-cast-function-type \
			-Wno-incompatible-pointer-types \
			-Wno-missing-braces \
			-Wno-unused-parameter \
			-gstabs \
			-D__USE_INLINE__ -D__USE_BASETYPE__ -g -D__amigaos4__ \
			-DUSE_DEBUG=$(USE_DEBUG)


LDFLAGS	= 

VERSION = 1

TARGET	= NallePUH
OBJECTS	= Nalle.o PUH.o debug.o iconify.o gui.o req.o file.o ciaa.o ciab.o emu_cia.o warn.o init.o timer.o

all:	make_locale $(TARGET) 

%.o:	%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(OUTPUT_OPTION) $<

make_locale: locale/NallePUH.c locale/NallePUH.h

locale/NallePUH.ct: locale/NallePUH.cd
	@$(CATCOMP) locale/NallePUH.cd CTFILE locale/NallePUH.ct

locale/NallePUH.h: locale/NallePUH.cd locale/NallePUH.ct
	@$(CATCOMP) locale/NallePUH.cd CFILE locale/NallePUH.h NOSTRINGS NOARRAY NOBLOCK NOCODE

locale/NallePUH.c: locale/NallePUH.cd locale/NallePUH.ct
	@$(CATCOMP) locale/NallePUH.cd CFILE locale/NallePUH.c NOBLOCK NOCODE

clean:
	$(RM) $(TARGET) $(OBJECTS)

$(TARGET): 	$(OBJECTS)
	$(CC) $(LDFLAGS) -o $@.debug $^
	$(STRIP) -R .comment $@.debug -o $@

.PHONY: revision
revision:
	bumprev $(VERSION) $(TARGET)

Nalle.o:	Nalle.c PUH.h locale/NallePUH.h

PUH.o:		PUH.c PUH.h

