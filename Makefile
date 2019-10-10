BIN=./bin/
SRC=./src/
LIB=./lib/
BUILD=./build/

MAKE=make
CC=gcc
CCFLAGS=-Wall -g
CLIBS=
LIBS=

$(BUILD)%.o.1:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -fPIC -o$@ $< $(CLIBS)

$(BUILD)%.o:$(SRC)%.c
	$(CC) $(CCFLAGS) -c -o$@ $< $(CLIBS)

lib%.so:$(BUILD)%.o.1
	$(CC) $(CCFLAGS) -shared -Wl,-soname,$@ -o$(LIB)$@ $< $(CLIBS)

# Build Make-Pony

MKPNY_LIBS=color target nbt
MKPNY_REQ=$(SRC)make-pony.c $(SRC)color.h $(SRC)target.h $(SRC)make-pony.h $(SRC)nbt.h

%.build:$(BUILD)%.o $(addsuffix .so, $(addprefix lib, $(LIBS)))
	$(CC) $(CCFLAGS) -o$* $< -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(LIBS) m) $(CLIBS)

%/:
	mkdir $@

all:
	+$(MAKE) make-pony
	+$(MAKE) thumbnailer

make-pony:build/ lib/ $(MKPNY_REQ) $(addsuffix .so, $(addprefix lib, $(MKPNY_LIBS))) $(BUILD)make-pony.o
	$(CC) $(CCFLAGS) -o$@ $(BUILD)$@.o -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(MKPNY_LIBS) m) $(CLIBS)

make-pony.exe:build/ $(MKPNY_REQ)
	+$(MAKE) -f Makefile.win32 make-pony.exe
	strip $@

make-pony.x86_64.exe:build/ $(MKPNY_REQ)
	+$(MAKE) -f Makefile.win64 make-pony.x86_64.exe
	strip $@

clean:
	-rm -rvf build/ lib/ *.exe make-pony thumbnailer
	cd libpng; $(MAKE) -f Makefile.win clean
	cd zlib; $(MAKE) -f Makefile.win clean

$(BUILD)nbt.o.1:$(SRC)nbt.c $(SRC)nbt.h
$(BUILD)color.o.1:$(SRC)color.c $(SRC)color.h
$(BUILD)target.o.1:$(SRC)target.c $(SRC)target.h
$(BUILD)make-pony.o:$(MKPNY_REQ)


THUMB_LIBS=pngimg pixel color nbt
THUMB_REQ=$(SRC)thumbnailer.c $(SRC)color.h $(SRC)pngimg.h $(SRC)pixel.h $(SRC)nbt.h
thumbnailer:build/ lib/ $(THUMB_REQ) $(addsuffix .so, $(addprefix lib, $(THUMB_LIBS))) $(BUILD)thumbnailer.o
	$(eval CLIBS += -lpng)
	$(CC) $(CCFLAGS) -o$@ $(BUILD)$@.o -Wl,-rpath,$(LIB) -L$(LIB) $(addprefix -l, $(THUMB_LIBS) m) $(CLIBS)

$(BUILD)pngimg.o.1:$(SRC)pngimg.c $(SRC)pngimg.h $(SRC)pixel.h
$(BUILD)pixel.o.1:$(SRC)pixel.c $(SRC)pixel.h
$(BUILD)thumbnailer.o:$(SRC)thumbnailer.c $(SRC)color.h $(SRC)pngimg.h $(SRC)pixel.h \
 $(SRC)nbt.h
