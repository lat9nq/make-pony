CC=gcc
CCFLAGS=-Wall -g

%.o:%.c
	$(CC) $(CCFLAGS) -c -o$@ $<

COOC_OFILES=convert_old_oc.o nbt.o color.o target.o
convert_old_oc:$(COOC_OFILES)
	$(CC) $(CCFLAGS) -o$@ $^ -lm

.PHONY:clean
clean:
	rm -vf *.o convert_old_oc


color.o: color.c color.h
convert_old_oc.o: convert_old_oc.c nbt.h color.h target.h
make-pony.o: make-pony.c color.h target.h make-pony.h nbt.h
nbt.o: nbt.c nbt.h color.h
target.o: target.c target.h

