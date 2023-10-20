
OPT=USE_DEBUG=0
OPT+=GUI_DEBUG=0

OPT_DEBUG=USE_DEBUG=0
OPT_DEBUG+=GUI_DEBUG=1

all: dirs normal debug

normal:
		make -f Makefile.NallePuh all $(OPT) TARGET=NallePuh obj_dir=objs/normal
		strip -R .comment NallePuh

debug:	
		make -f Makefile.NallePuh all $(OPT_DEBUG) TARGET=NallePuh.debug  obj_dir=objs/debug

clean:
		make -f Makefile.NallePuh TARGET=NallePuh  obj_dir=objs/normal clean
		make -f Makefile.NallePuh TARGET=NallePuh.debug  obj_dir=objs/debug clean
		rm NallePuh NallePuh.debug

dirs:	objs objs/debug objs/normal

objs:
		makedir objs

objs/debug:
		makedir objs/debug

objs/normal:
		makedir objs/normal
