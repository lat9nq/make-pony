CC=gcc
CCFLAGS=-Wall -g

%.o:%.c
	$(CC) $(CCFLAGS) -c -o$@ $< -lpng

COOC_OFILES=convert_old_oc.o nbt.o color.o target.o
convert_old_oc:$(COOC_OFILES)
	$(CC) $(CCFLAGS) -o$@ $^ -lm

.PHONY:clean
clean:
	rm -vf *.o convert_old_oc

test_pngimg:test_pngimg.o pngimg.o pixel.o
	$(CC) $(CCFLAGS) -o$@ test_pngimg.o pngimg.o pixel.o -lpng

color.o: color.c color.h
convert_old_oc.o: convert_old_oc.c nbt.h color.h target.h
lex.yy.o: lex.yy.c
make-pony.o: make-pony.c color.h target.h make-pony.h nbt.h
nbt.o: nbt.c nbt.h color.h
pixel.o: pixel.c pixel.h
pngimg.o: pngimg.c pngimg.h pixel.h
render.o: render.c
target.o: target.c target.h
target_new.o: target_new.c target.h
target_old.o: target_old.c target.h
test_pngimg.o: test_pngimg.c pngimg.h pixel.h
