
CC			= ppc-amigaos-gcc
LD			= ppc-amigaos-ld
STRIP		= ppc-amigaos-strip
CATCOMP		= catcomp

CFLAGS	= -fomit-frame-pointer -O2 -W -Wall \
			-Wno-cast-function-type \
			-Wno-incompatible-pointer-types \
			-Wno-missing-braces \
			-D__USE_INLINE__ -D__USE_BASETYPE__ -g


LDFLAGS	= -g

TARGET	= NallePUH
OBJECTS	= Nalle.o PUH.o debug.o gui.o

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

Nalle.o:	Nalle.c PUH.h locale/NallePUH.h

PUH.o:		PUH.c PUH.h

