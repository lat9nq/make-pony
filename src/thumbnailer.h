#ifndef THUMBNAILER_H
#define THUMBNAILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>

#include "color.h"
#include "pngimg.h"
#include "nbt.h"

/* Define memory management maximums for mane and body details. */
#define DETAIL_MAX	6
#define COLOR_MAX	6
#define BODY_DETAIL_MAX	8

/* Sock definitions for determining what kind of sock */
#define SOCKS_NEW_MODEL	1
#define SOCKS_MODEL	2
#define SOCKS_TEXTURE	4

/* Should we print out targets instead of thumbnailing? */
#define PRINT_TARGETS	0

/* For determining if using separated eyes */
typedef enum {
	BOTH = 0,
	LEFT = 1,
	RIGHT = 2,
} which_eye_t;

/* Pony race */
typedef enum {
	EARTH = 0,
	PEGASUS = 1,
	UNICORN = 2,
	ALICORN = 4,
} race_t;

int style_color_count(char * style); /* Mane/Tail color count */
int style_detail_count(char * style); /* Mane/Tail detail count */
int sgetnum(char * s); /* Extract a number from a string */
void strtolower(char * s); /* Converts ASCII characters in a string to their lowercase counterpart */
void read_nbt_string(char * s, int fd); /* Read a NBT string from a file */
int get_nbt_string(char * s, uint8_t * data); /* Extract a NBT string from data */

int thumbnail(PNGIMG * canvas, uint8_t * data, uint32_t data_len);
int thumbnail_add_part(PNGIMG ** img, char * filename, color * c, float value);

/* Read 4 bytes and be able to use it as a different integer, or convert it from little to big endian */
typedef union {
	float f;
	uint8_t a[4];
} float_bin;

#endif
