CC=gcc
CCFLAGS=-Wall -g
LIBFLAGS=-lm

.c.o:
	$(CC) $(CCFLAGS) -c $< $(LIBFLAGS)

OFILES=color.o make-pony.o
make-pony:$(OFILES)
	$(CC) $(CCFLAGS) -o make-pony $(OFILES) $(LIBFLAGS)

%.strip:%.exe
	strip -Xxwg $<

color.o:color.c color.h
make-pony.o:make-pony.c color.h
