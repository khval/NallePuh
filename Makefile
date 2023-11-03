
CATCOMP		= catcomp

OPT=USE_DEBUG=0
OPT+=GUI_DEBUG=0

OPT_DEBUG=USE_DEBUG=0
OPT_DEBUG+=GUI_DEBUG=1

VERSION = 1

all: make_locale dirs normal debug tools/utf8-to-ascii

normal:
		make -f Makefile.NallePuh all $(OPT) TARGET=NallePuh obj_dir=objs/normal
		strip -R .comment NallePuh

debug:	
		make -f Makefile.NallePuh all $(OPT_DEBUG) TARGET=NallePuh.debug  obj_dir=objs/debug

clean:
		make -f Makefile.NallePuh TARGET=NallePuh  obj_dir=objs/normal clean
		make -f Makefile.NallePuh TARGET=NallePuh.debug  obj_dir=objs/debug clean
		rm NallePuh NallePuh.debug

make_locale: locale/NallePUH.c locale/NallePUH.h

locale/NallePUH.ct: locale/NallePUH.cd
	@$(CATCOMP) locale/NallePUH.cd CTFILE locale/NallePUH.ct

locale/NallePUH.h: locale/NallePUH.cd
	@$(CATCOMP) locale/NallePUH.cd CFILE locale/NallePUH.h NOSTRINGS NOARRAY NOBLOCK NOCODE

locale/NallePUH.c: locale/NallePUH.cd
	@$(CATCOMP) locale/NallePUH.cd CFILE locale/NallePUH.c NOBLOCK NOCODE


dirs:	objs objs/debug objs/normal

objs:
		makedir objs

objs/debug:
		makedir objs/debug

objs/normal:
		makedir objs/normal

tools/utf8-to-ascii: tools/utf8-to-ascii.c
		gcc tools/utf8-to-ascii.c -o tools/utf8-to-ascii

.PHONY: revision
revision:
	bumprev $(VERSION) NallePuh
