
OPT=USE_DEBUG=0
OPT+=GUI_DEBUG=0

OPT_DEBUG=USE_DEBUG=0
OPT_DEBUG+=GUI_DEBUG=1


quick:
		make -f Makefile.NallePuh all $(OPT) TARGET=NallePuh
		strip -R .comment NallePuh

all: normal debug

normal:
		make -f Makefile.NallePuh clean 
		make -f Makefile.NallePuh all $(OPT) TARGET=NallePuh
		strip -R .comment NallePuh

debug:	
		make -f Makefile.NallePuh clean 
		make -f Makefile.NallePuh all $(OPT_DEBUG) TARGET=NallePuh.debug

clean:
		make -f Makefile.NallePuh clean
		rm NallePuh NallePuh.debug

