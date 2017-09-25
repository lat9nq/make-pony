CC=gcc
CCFLAGS=-Wall -O2#-g
LIBFLAGS=-lm

.c.o:
	$(CC) $(CCFLAGS) -c $< $(LIBFLAGS)

all:
	make make-pony

commit:
	make make-pony
	git commit -a

OFILES=color.o make-pony.o
make-pony:$(OFILES)
	$(CC) $(CCFLAGS) -o make-pony $(OFILES) $(LIBFLAGS)

%.strip:%.exe
	strip -Xxwg $<

clean:
	rm -vf *.o *.txt .*.swp make-pony make-pony.exe

color.o:color.c color.h
make-pony.o:make-pony.c color.h
