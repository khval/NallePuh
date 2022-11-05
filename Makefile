
CC			= ppc-amigaos-gcc
LD			= ppc-amigaos-ld
STRIP		= ppc-amigaos-strip
CFLAGS	= -fomit-frame-pointer -O2 -W -Wall -Wno-unused \
			  -DNO_GUI -D__USE_INLINE__ -D__USE_BASETYPE__ -g
LDFLAGS	= -lauto -g

TARGET	= NallePUH
OBJECTS	= Nalle.o PUH.o debug.o

%.o:	%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $(OUTPUT_OPTION) $<

all:	$(TARGET)


mostly-clean:
	$(RM) $(OBJECTS)

clean:
	$(RM) $(TARGET) $(OBJECTS)

$(TARGET): 	$(OBJECTS)
	$(CC) $(LDFLAGS) -o $@.debug $^
	$(STRIP) -R .comment $@.debug -o $@

Nalle.o:	Nalle.c PUH.h NallePUH.h

PUH.o:		PUH.c PUH.h

