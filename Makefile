BIN=./bin/
SRC=./src/
LIB=./lib/
BUILD=./build/

MAKE=make
CC=gcc
CCFLAGS=-Wall -g

# Build Make-Pony

MKPNY_LIBS=color target nbt
MKPNY_REQ=$(SRC)make-pony.c $(SRC)color.h $(SRC)target.h $(SRC)make-pony.h $(SRC)nbt.h

$(BUILD)%.o.1:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -fPIC -o$@ $< -lpng

$(BUILD)%.o:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -o$@ $< -lpng

lib%.so:$(BUILD)%.o.1
	$(CC) $(CCFLAGS) -shared -Wl,-soname,$@ -o$(LIB)$@ $< -lpng

%.build:$(BUILD)%.o $(addsuffix .so, $(addprefix lib, $(MKPNY_LIBS)))
	$(CC) $(CCFLAGS) -o$* $< -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(MKPNY_LIBS) m) -lpng

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
$(BUILD)pngimg.o.1:$(SRC)pngimg.c $(SRC)pngimg.h $(SRC)pixel.h
$(BUILD)pixel.o.1:$(SRC)pixel.c $(SRC)pixel.h
$(BUILD)make-pony.o:$(MKPNY_REQ)

