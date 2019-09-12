BIN=./bin/
SRC=./src/
LIB=./lib/
BUILD=./build/

MAKE=make
CC=gcc
CCFLAGS=-Wall -g
CLIBS=

$(BUILD)%.o.1:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -fPIC -o$@ $< $(CLIBS)

$(BUILD)%.o:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -o$@ $< $(CLIBS)

lib%.so:$(BUILD)%.o.1
	$(CC) $(CCFLAGS) -shared -Wl,-soname,$@ -o$(LIB)$@ $< $(CLIBS)

# Build Make-Pony

MKPNY_LIBS=color target nbt
MKPNY_REQ=$(SRC)make-pony.c $(SRC)color.h $(SRC)target.h $(SRC)make-pony.h $(SRC)nbt.h

%.build:$(BUILD)%.o $(addsuffix .so, $(addprefix lib, $(MKPNY_LIBS)))
	$(CC) $(CCFLAGS) -o$* $< -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(MKPNY_LIBS) m) $(CLIBS)

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

$(BUILD)nbt.o.1:$(SRC)nbt.c $(SRC)nbt.h
$(BUILD)color.o.1:$(SRC)color.c $(SRC)color.h
$(BUILD)target.o.1:$(SRC)target.c $(SRC)target.h
$(BUILD)make-pony.o:$(MKPNY_REQ)


CLIBS +=-lpng
THUMB_LIBS=pngimg pixel

$(BUILD)pngimg.o.1:$(SRC)pngimg.c $(SRC)pngimg.h $(SRC)pixel.h
$(BUILD)pixel.o.1:$(SRC)pixel.c $(SRC)pixel.h
