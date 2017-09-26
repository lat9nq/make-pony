LEX=flex

CC=gcc
CCFLAGS=-Wall -O2#-g
LIBFLAGS=-lm

.l.c:
	$(LEX) -o$@ $<

.c.o:
	$(CC) $(CCFLAGS) -c $< $(LIBFLAGS)

all:
	make make-pony

commit:
	make make-pony
	git commit -a

OFILES=color.o make-pony.o target.o
make-pony:$(OFILES)
	$(CC) $(CCFLAGS) -o make-pony $(OFILES) $(LIBFLAGS)

%.strip:%.exe
	strip -Xxwg $<

%:%.o
	$(CC) $(CCFLAGS) $< -o$@ -LC:\Program\ Files\ \(x86\)\GnuWin32\lib -lfl
	
clean:
	rm -vf *.o *.txt .*.swp make-pony make-pony.exe

color.o: color.c color.h
make-pony.o: make-pony.c color.h target.h
target.o: target.c target.h
