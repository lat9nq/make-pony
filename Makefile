CC=gcc
CCFLAGS=-Wall -O3 -pipe -pthread
LIBFLAGS=-lm -lpng `pkg-config --cflags --libs gtk+-3.0 libpng16`

SRC=./src/
BUILD=./build/

%/:
	mkdir $@

$(BUILD)%.o:$(SRC)%.c
	$(CC) $(CCFLAGS) -c $< -o$@ $(LIBFLAGS)

.PHONY: all
all:
	+make -f $(abspath $(lastword $(MAKEFILE_LIST))) make-pony
	+make -f $(abspath $(lastword $(MAKEFILE_LIST))) thumbnailer

MKOFILES=$(BUILD)make-pony.o $(BUILD)target.o $(BUILD)color.o $(BUILD)nbt.o $(BUILD)pngimg.o $(BUILD)pixel.o
make-pony:build/ $(MKOFILES)
	$(CC) $(CCFLAGS) $(MKOFILES) -o$@ $(LIBFLAGS) -rdynamic

THUMB_OFILES=$(BUILD)color.o $(BUILD)nbt.o $(BUILD)pixel.o $(BUILD)pngimg.o $(BUILD)thumbnailer.o $(BUILD)thumbnailer_main.o
thumbnailer:build/ $(THUMB_OFILES)
	$(CC) $(CCFLAGS) $(THUMB_OFILES) -o$@ $(LIBFLAGS)

$(BUILD)color.o: $(SRC)color.c $(SRC)color.h
$(BUILD)make-pony.o: $(SRC)make-pony.c $(SRC)color.h $(SRC)target.h $(SRC)make-pony.h \
 $(SRC)nbt.h $(SRC)pngimg.h $(SRC)pixel.h
$(BUILD)nbt.o: $(SRC)nbt.c $(SRC)nbt.h $(SRC)color.h
$(BUILD)pixel.o: $(SRC)pixel.c $(SRC)pixel.h
$(BUILD)pngimg.o: $(SRC)pngimg.c $(SRC)pngimg.h $(SRC)pixel.h $(SRC)color.h
$(BUILD)target.o: $(SRC)target.c $(SRC)target.h
$(BUILD)thumbnailer.o: $(SRC)thumbnailer.c $(SRC)color.h $(SRC)pngimg.h $(SRC)pixel.h \
 $(SRC)nbt.h $(SRC)thumbnailer.h
$(BUILD)thumbnailer_main.o: $(SRC)thumbnailer_main.c $(SRC)color.h src/pngimg.h \
 src/pixel.h src/nbt.h src/thumbnailer.h

.PHONY:clean
clean:
	-rm -rvf build/ thumbnailer thumbnailer.exe make-pony make-pony.exe
