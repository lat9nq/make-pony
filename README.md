# make-pony
A generator for Pony Player Models Second Generation

Credits for [PPM/2](https://git.dbot.serealia.ca/Ponyscape-open/PPM2) goes to DBot

It is best to use this program with the command prompt (Windows) or the terminal emulator (Linux).
Use `make-pony -h` for help

To use with Windows:
- Download the executable
- Run from explorer (generates a single file)
- Or run from command prompt for more options (see -h)

To compile for Linux:
- `tar -xf make-pony-1.0.tar.gz`
- `cd make-pony-1.0`
- `make`

The thumbnailer isn't possible without:
- [libpng](http://www.libpng.org/pub/png/libpng.html) and
- [zlib](https://www.zlib.net/)

Changelog:
v1.4
- Added the thumbnailer, which is not complete but for many OC's is compatible and produces colorful results.
- The main program is compatible with changes to PPM/2 from over a year ago, namely the conversion from .txt, JSON-like formats to a proprietary "NBT" format.

v1.3
- Added a simplified mode that reduces detail usage and styles outputted
- Updated `-m` to work more efficiently
- `-r` tells it to generate a random color on the mane
- Body color generation is updated
- Bug fixes

v1.2
- Added support for generated ponies that have completely desaturated hair
- Added new switch
- `-k` now controls hair style
- Generates a full PPM/2 model file
- Bug fixes

v1.1
- Added males
- You can specify a male with `-g`
- `-x` now controls the color, not `-c`
- `-c` outputs to STDOUT
- Updated help text
