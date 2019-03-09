BIN=./bin/
SRC=./src/
LIB=./lib/
BUILD=./build/

MAKE=make

LIBS=color target

CC=gcc
CCFLAGS=-Wall -g

MKPNY_REQ=$(SRC)make-pony.c $(SRC)color.h $(SRC)target.h $(SRC)make-pony.h

$(BUILD)%.o.1:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -fPIC -o$@ $<

$(BUILD)%.o:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -o$@ $<

lib%.so:$(BUILD)%.o.1
	$(CC) $(CCFLAGS) -shared -Wl,-soname,$@ -o$(LIB)$@ $<

%.build:$(BUILD)%.o $(addsuffix .so, $(addprefix lib, $(LIBS)))
	$(CC) $(CCFLAGS) -o$* $< -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(LIBS) m)

%/:
	mkdir $@

make-pony:build/ lib/ $(MKPNY_REQ)
	+$(MAKE) make-pony.build

make-pony.exe:build/ $(MKPNY_REQ)
	+$(MAKE) -f Makefile.win32 make-pony.exe
	strip $@

make-pony.x86_64.exe:build/ $(MKPNY_REQ)
	+$(MAKE) -f Makefile.win64 make-pony.x86_64.exe
	strip $@

clean:
	-rm -rvf build/ lib/ *.exe make-pony

$(BUILD)color.o.1:$(SRC)color.c $(SRC)color.h
$(BUILD)target.o.1:$(SRC)target.c $(SRC)target.h
$(BUILD)make-pony.o:$(MKPNY_REQ)

