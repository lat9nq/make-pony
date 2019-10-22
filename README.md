# make-pony
A generator for Pony Player Models Second Generation

![Linux screenshot](https://i.imgur.com/ov8LUnw.png)

Credits for [PPM/2](https://git.dbot.serealia.ca/Ponyscape-open/PPM2) goes to DBot

This project is made possible by:
- [MSYS2](https://www.msys2.org/) (for Windows support),
- [GTK+](https://www.gtk.org/), and
- [libpng](http://www.libpng.org/pub/png/libpng.html)

Changelog:

v2.0-rc4
- Fixes additional memory leaks (remaining leaks are out of my control) 
- Missing manes/tails for make-pony styles are installed
- Changed style list
- PNGs are optimised for speed mostly
- Down-scaled preview in GUI now averages pixels rather than sampling
- Other optimizations

v2.0-rc3
- Fixes a really nasty memory leak in the thumbnailer

v2.0-rc2
- Windows compatibility issue with generated files
- Adds error message when trying to use non-existant directories

v2.0-rc1
- Adds GTK+ support to Make-pony in both Linux and Windows
	- Integrates the thumbnailer into Make-pony
- Hue selection is less restricted, and saturation/value selection is unlocked
- Fixed where certain switches cause a seed to produce vastly different results

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
