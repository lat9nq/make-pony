/* thumbnailer.c
 * 
 * A thumbnailer for Pony Player Models Second Generation (by dbotthepone).
 * 
 * Supports any hairstyle defined at style_{color,detail}_count, and any body
 * detail. Also supports socks (old and new models only).
 * 
 * Requires libpng, and its dependency zlib.
 */

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

#pragma GCC diagnostic ignored "-Wformat-overflow="

/* Define memory management maximums for mane and body details. */
#define DETAIL_MAX	6
#define COLOR_MAX	6
#define BODY_DETAIL_MAX	8

/* Sock definitions for determining what kind of sock */
#define SOCKS_NEW_MODEL	1
#define SOCKS_MODEL	2
#define SOCKS_TEXTURE	4

/* Should we print out targets instead of thumbnailing? */
#define PRINT_TARGETS	1

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

/* Read 4 bytes and be able to use it as a different integer, or convert it from little to big endian */
typedef union {
	float f;
	uint8_t a[4];
} float_bin;

int main(int argc, char * argv[]) {
	/* Read the first and only the first argument as the input file */
	if (argc < 2) {
		printf("expected 2 arguments, found %d\n", argc);
		printf("usage: %s <filename.dat>\n", argv[0]);
		return 0;
	}

	/* Point to the filename */
	char * filename;
	filename = argv[1];
	int fd;
	/* Open a file descriptor to the file */
#ifdef _WIN64
	/* In Windows, we must open it as a binary or risk an early file close */
	fd = open(filename, O_RDONLY | O_BINARY);
#else
	fd = open(filename, O_RDONLY);
#endif

	/* If we can't read the file, exit */
	if (fd == -1) {
		printf("error: %s isn't accessible\n", filename);
		return 0;
	}

	nbt_t nbt; // main NBT, using as a header for an array of NBTs
	int nbt_len, // max allocated length of the NBT data list
		nbt_at; // "where are we" in the NBT data
	nbt_len = 128;
	nbt_at = 0;

	/* Initialize the base NBT element */
	nbt.code = NBT_GROUP;
	nbt.name_length = 4;
	strncpy(nbt.name, "data", nbt.name_length);
	nbt.payload = malloc(sizeof(nbt_t) * nbt_len);

	color body_color;
	color body_detail[BODY_DETAIL_MAX]; // array of body detail colors
	color tail_color[6];
	color mane_color[6];
	color tail_detail[6];
	color mane_detail[6];
	uint8_t use_separated_eyes; // boolean
	/* For eye colors, we use an array of 3 for the both, left, and right colors
	 * We don't know ahead of time whether we should keep just the both, or left
	 * and right colors since there is no order in an NBT.
	 */
	color eye_sclera[3];
	color eye_iris[3];
	color eye_iris_gradient[3];
	color eye_irisline1[3];
	color eye_irisline2[3];
	color eye_reflection[3];
	color eye_pupil[3];
	color eye_brows;
	color eye_lashes;
	/* We initialize brow and eyelash colors since older OCs don't have a
	 * color setting for these elements.
	 */
	color_init(&eye_brows); 
	color_init(&eye_lashes);
	eye_lashes.a = 255;
	eye_brows.a = 255;
	color socks_new[3];
	color socks_model;
	which_eye_t which_eye = BOTH;
	uint8_t use_eyelashes = 1, use_socks = 0;
	char uppermane[32];
	char lowermane[32];
	char tail[32];
	char * body_detail_s[BODY_DETAIL_MAX];
	race_t race = EARTH;
	body_color.r = 0;
	body_color.g = 0;
	body_color.b = 0;
	/* Assume Derpy's mane/tail unless otherwise specified */
	strcpy(uppermane, "mailcall");
	strcpy(lowermane, "mailcall");
	strcpy(tail, "mailcall");

	uint8_t c; // temporary character
	char s[256]; // temporary string
	uint32_t l; // temporary place to store 4 bytes
	float_bin f; // converts endianess and extracts colors
	//int print_c = 0;

	/* Initialize body detail strings to blank */
	for (int i = 0; i < BODY_DETAIL_MAX; i++) {
		body_detail_s[i] = NULL;
	}

	/* Read input as long as there is data left in the file */
	while (read(fd, &c, 1)) {
		l = 0;
		s[0] = 0;
		switch (c) {
			case NBT_GROUP:
				read_nbt_string(s, fd);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", GRUP);\n", s);
			break;
			case NBT_INT:
				read_nbt_string(s, fd);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", INT);\n", s);
			case NBT_FLOAT:
				if (c == NBT_FLOAT)
					read_nbt_string(s, fd);
				/*if (!strcmp(s, "tattoo_rotate_5")) {
					printf("%s\n", s);
				}*/
				if (c == NBT_FLOAT && PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", VAL);\n", s);
				//printf("%s\n", s);
				read(fd, &f.a[3], 1);
				read(fd, &f.a[2], 1);
				read(fd, &f.a[1], 1);
				read(fd, &f.a[0], 1);
			break;
			case NBT_STRING:
				read_nbt_string(s, fd);
				int save = 0;
				int n;
				if (!strcmp("mane_new",s))
					save = 1;
				else if (!strcmp("manelower_new",s))
					save = 2;
				else if (!strcmp("tail_new",s))
					save = 3;
				else if (!strcmp("race",s))
					save = 4;
				else if (!strcmp("eyelash",s))
					save = 5;
				else if (strstr(s, "body_detail") && !strstr(s, "url")) {
					save = 6;
					//puts(s);
					n = sgetnum(s) - 1;
				}
			/*	else if (!strcmp("eye_url_right",s)) {
					printf("%s\n", s);
					print_c = 1;
				}*/
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", STR);\n", s);
				read_nbt_string(s, fd);
				switch (save) {
					case 1: // mane_new
						strcpy(uppermane, s);
					break;
					case 2: // manelower_new
						strcpy(lowermane, s);
					break;
					case 3: // tail_new
						strcpy(tail, s);
					break;
					case 4: // race
						strtolower(s);
						if (!strcmp(s, "unicorn")) {
							race = UNICORN;
						}
						else if (!strcmp(s, "pegasus")) {
							race = PEGASUS;
						}
						else if (!strcmp(s, "alicorn")) {
							race = ALICORN;
						}
					break;
					case 5:
						strtolower(s);
						if (!strcmp(s, "none")) {
							use_eyelashes = 0;
						}
					break;
					case 6:
						if (strcmp(s, "NONE")) {
							body_detail_s[n] = (char *)malloc(sizeof(char) * 32);
							strtolower(s);
							strcpy(body_detail_s[n], s);
							//puts(s);
						}
					break;
				}
			break;
			case NBT_BOOLEAN:
				read_nbt_string(s, fd);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", BOOL);\n", s);
				read(fd, &c, 1);
				if (!strcmp(s, "separate_eyes")) {
					use_separated_eyes = c;
				}
				else if (!strcmp(s, "socks_model_new") && !use_socks) {
					use_socks = c * SOCKS_NEW_MODEL;
				}
				else if (!strcmp(s, "socks_model") && !use_socks) {
					use_socks = c * SOCKS_MODEL;
				}
			break;
			case NBT_COLOR:
				read_nbt_string(s, fd);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", COL);\n", s);
				read(fd, &l, 4);
				read(fd, &f, 4);

				if (!strcmp(s,"body")) {
					body_color.r = f.a[0];
					body_color.g = f.a[1];
					body_color.b = f.a[2];
					body_color.a = f.a[3];
				}
				else if (strstr(s, "body")) {
					if (strstr(s,"detail") && ! strstr(s, "url")) {
						int n;
						n = sgetnum(s) - 1;
						body_detail[n].r = f.a[0] + 128;
						body_detail[n].g = f.a[1] + 128;
						body_detail[n].b = f.a[2] + 128;
						body_detail[n].a = f.a[3] + 128;
					}
				}
				else if (strstr(s,"eye")) {
					//printf("%s\n", s);
					which_eye_t use_eye = BOTH;
					if (strstr(s, "_left")) {
						use_eye = LEFT;
					}
					else if (strstr(s, "_right")) {
						use_eye = RIGHT;
					}
					if (strstr(s,"eye_bg")) {
						eye_sclera[use_eye].r = f.a[0] + 128;
						eye_sclera[use_eye].g = f.a[1] + 128;
						eye_sclera[use_eye].b = f.a[2] + 128;
						eye_sclera[use_eye].a = f.a[3] + 128;
						//printf("%s %02x %02x %02x %02x\n", s, f.a[0], f.a[1], f.a[2], f.a[3]);
					}
					else if (strstr(s, "eye_irisline1")) {
						eye_irisline1[use_eye].r = f.a[0] + 128;
						eye_irisline1[use_eye].g = f.a[1] + 128;
						eye_irisline1[use_eye].b = f.a[2] + 128;
						eye_irisline1[use_eye].a = f.a[3] + 128;
					}
					else if (strstr(s, "eye_irisline2")) {
						eye_irisline2[use_eye].r = f.a[0] + 128;
						eye_irisline2[use_eye].g = f.a[1] + 128;
						eye_irisline2[use_eye].b = f.a[2] + 128;
						eye_irisline2[use_eye].a = f.a[3] + 128;
					}
					else if (strstr(s,"eye_iris1")) {
						eye_iris[use_eye].r = f.a[0] + 128;
						eye_iris[use_eye].g = f.a[1] + 128;
						eye_iris[use_eye].b = f.a[2] + 128;
						eye_iris[use_eye].a = f.a[3] + 128;
					}
					else if (strstr(s,"eye_iris2")) {
						eye_iris_gradient[use_eye].r = f.a[0] + 128;
						eye_iris_gradient[use_eye].g = f.a[1] + 128;
						eye_iris_gradient[use_eye].b = f.a[2] + 128;
						eye_iris_gradient[use_eye].a = f.a[3] + 128;
					}
					else if (strstr(s, "eye_reflection")) {
						eye_reflection[use_eye].r = f.a[0] + 128;
						eye_reflection[use_eye].g = f.a[1] + 128;
						eye_reflection[use_eye].b = f.a[2] + 128;
						eye_reflection[use_eye].a = f.a[3] + 128;
					}
					else if (strstr(s, "hole")) {
						eye_pupil[use_eye].r = f.a[0] + 128;
						eye_pupil[use_eye].g = f.a[1] + 128;
						eye_pupil[use_eye].b = f.a[2] + 128;
						eye_pupil[use_eye].a = f.a[3] + 128;
						//printf("%02x %02x %02x %02s\n", f.a[0], f.a[1], f.a[2], f.a[3]);
					}
					else if (strstr(s,"brows")) {
						eye_brows.r = f.a[0] + 128;
						eye_brows.g = f.a[1] + 128;
						eye_brows.b = f.a[2] + 128;
						eye_brows.a = f.a[3] + 128;
					}
					else if (strstr(s, "lash_")) {
						eye_lashes.r = f.a[0] + 128;
						eye_lashes.g = f.a[1] + 128;
						eye_lashes.b = f.a[2] + 128;
						eye_lashes.a = f.a[3] + 128;
					}
				}
				else if (strstr(s,"detail")) {
					if (strstr(s,"body") && !strstr(s,"url")) {
						body_detail[sgetnum(s)-1].r = f.a[0]+ 128;
						body_detail[sgetnum(s)-1].g = f.a[1]+ 128;
						body_detail[sgetnum(s)-1].b = f.a[2]+ 128;
						body_detail[sgetnum(s)-1].a = f.a[3]+ 128;
					}
					else if (strstr(s,"mane") && !strstr(s,"url")
							&& !strstr(s,"upper") && !strstr(s,"lower")) {
						mane_detail[sgetnum(s)-1].r = f.a[0]+ 128;
						mane_detail[sgetnum(s)-1].g = f.a[1]+ 128;
						mane_detail[sgetnum(s)-1].b = f.a[2]+ 128;
						mane_detail[sgetnum(s)-1].a = f.a[3]+ 128;
					}
					else if (strstr(s,"tail") == s && !strstr(s,"url")) {
						tail_detail[sgetnum(s)-1].r = f.a[0]+ 128;
						tail_detail[sgetnum(s)-1].g = f.a[1]+ 128;
						tail_detail[sgetnum(s)-1].b = f.a[2]+ 128;
						tail_detail[sgetnum(s)-1].a = f.a[3]+ 128;
						//printf("%s %02x %02x %02x\n", s, tail_detail[sgetnum(s)-1].r, tail_detail[sgetnum(s)-1].g, tail_detail[sgetnum(s)-1].b);
					}
				}
				else if (strstr(s, "socks")) {
					if (strstr(s, "socks_new_model")) {
						socks_new[sgetnum(s)-1].r = f.a[0] + 128;
						socks_new[sgetnum(s)-1].g = f.a[1] + 128;
						socks_new[sgetnum(s)-1].b = f.a[2] + 128;
						socks_new[sgetnum(s)-1].a = 255;
						//printf("%s %d %02x %02x %02x\n", s, sgetnum(s), f.a[0], f.a[1], f.a[2]);
					}
					else if (strstr(s, "socks_model")) {
						socks_model.r = f.a[0] + 128;
						socks_model.g = f.a[1] + 128;
						socks_model.b = f.a[2] + 128;
						socks_model.a = 255;
					}
				}
				else if (strstr(s,"color")) {
					if (strstr(s,"mane") && !strstr(s,"url")
							&& !strstr(s,"upper") && !strstr(s,"lower")) {
						mane_color[sgetnum(s)-1].r = f.a[0] + 128;
						mane_color[sgetnum(s)-1].g = f.a[1] + 128;
						mane_color[sgetnum(s)-1].b = f.a[2] + 128;
						mane_color[sgetnum(s)-1].a = f.a[3] + 128;
					}
					else if (strstr(s,"tail") && !strstr(s,"url")) {
						tail_color[sgetnum(s)-1].r = f.a[0] + 128;
						tail_color[sgetnum(s)-1].g = f.a[1] + 128;
						tail_color[sgetnum(s)-1].b = f.a[2] + 128;
						tail_color[sgetnum(s)-1].a = f.a[3] + 128;
					}
				}

				int seen = 0;
				for (int i = 0; i < nbt_at; i++) {
					color * x;
					x = ((nbt_t*)(nbt.payload))[i].payload;
					if (x->r == f.a[0]
							&& x->g == f.a[1]
							&& x->b == f.a[2]) {
							//&& x->a == f.a[3]) {
						seen = 1;
						break;
					}
				}
				if (!seen) {
					((nbt_t *)(nbt.payload))[nbt_at].code = NBT_COLOR;
					((nbt_t *)(nbt.payload))[nbt_at].name_length = strlen(s);
					strcpy(((nbt_t *)(nbt.payload))[nbt_at].name, s);
					color * col = (color *)malloc(sizeof(*col));
					col->r = f.a[0];
					col->g = f.a[1];
					col->b = f.a[2];
					col->a = f.a[3];
					((nbt_t *)(nbt.payload))[nbt_at].payload = col;
					nbt_at++;
				}
			break;
		}
	}
	
	if (PRINT_TARGETS)
			exit(0);

	int width, height;
	width = 1024;
	height = 1024;

	PNGIMG * canvas;
	canvas = pngimg_init();
	pngimg_alloc(canvas, width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			pngimg_at(canvas, i, j)->r = body_color.r + 128;
			pngimg_at(canvas, i, j)->g = body_color.g + 128;
			pngimg_at(canvas, i, j)->b = body_color.b + 128;
			pngimg_at(canvas, i, j)->a = 255;
		}
	}

	Pixel * can_col;
	for (int k = 0; k < nbt_at; k++) {
		color * nbt_col = ((nbt_t *)(nbt.payload))[k].payload;
		for (int i = (k % 8) * 128; i < (k % 8 + 1) * 128; i++) {
			for (int j = (k / 8) * 128; j < (k / 8 + 1) * 128; j++) {
				can_col = pngimg_at(canvas, i, j);
				can_col->r = nbt_col->r + 128;
				can_col->g = nbt_col->g + 128;
				can_col->b = nbt_col->b + 128;
			}
		}
	}

	PNGIMG * canvas_body_outline;
	PNGIMG * canvas_body_fill;
	PNGIMG * canvas_body_detail[BODY_DETAIL_MAX];
	PNGIMG * canvas_ear_outline;
	PNGIMG * canvas_ear_fill;
	PNGIMG * canvas_uppermane_outline;
	PNGIMG * canvas_uppermane_fill;
	PNGIMG * canvas_uppermane_color[COLOR_MAX];
	PNGIMG * canvas_uppermane_color_outline[COLOR_MAX];
	PNGIMG * canvas_uppermane_detail[DETAIL_MAX];
	PNGIMG * canvas_horn_fill = pngimg_init();
	PNGIMG * canvas_horn_outline = pngimg_init();
	PNGIMG * canvas_lowermane_outline;
	PNGIMG * canvas_lowermane_fill;
	PNGIMG * canvas_lowermane_color[COLOR_MAX];
	PNGIMG * canvas_lowermane_color_outline[COLOR_MAX];
	PNGIMG * canvas_lowermane_detail[DETAIL_MAX];
	PNGIMG * canvas_tail_outline;
	PNGIMG * canvas_tail_fill;
	PNGIMG * canvas_tail_color[COLOR_MAX];
	PNGIMG * canvas_tail_color_outline[COLOR_MAX];
	PNGIMG * canvas_tail_detail[DETAIL_MAX];
	PNGIMG * canvas_wing_fill = pngimg_init();
	PNGIMG * canvas_wing_outline = pngimg_init();

	PNGIMG * canvas_eye_iris = pngimg_init();
	PNGIMG * canvas_eye_iris_gradient = pngimg_init();
	PNGIMG * canvas_eye_irisline1 = pngimg_init();
	PNGIMG * canvas_eye_irisline2 = pngimg_init();
	PNGIMG * canvas_eye_pupil = pngimg_init();
	PNGIMG * canvas_eye_reflection = pngimg_init();
	PNGIMG * canvas_eye_sclera = pngimg_init();
	PNGIMG * canvas_eye_brows = pngimg_init();
	PNGIMG * canvas_eye_lashes = pngimg_init();

	PNGIMG * canvas_reye_iris;
	PNGIMG * canvas_reye_iris_gradient;
	PNGIMG * canvas_reye_irisline1;
	PNGIMG * canvas_reye_irisline2;
	PNGIMG * canvas_reye_pupil;
	PNGIMG * canvas_reye_reflection;
	PNGIMG * canvas_reye_sclera;
	if (use_separated_eyes) {
		canvas_reye_iris = pngimg_init();
		canvas_reye_iris_gradient = pngimg_init();
		canvas_reye_irisline1 = pngimg_init();
		canvas_reye_irisline2 = pngimg_init();
		canvas_reye_pupil = pngimg_init();
		canvas_reye_reflection = pngimg_init();
		canvas_reye_sclera = pngimg_init();
	}

	canvas_body_outline = pngimg_init();
	canvas_body_fill = pngimg_init();
	canvas_ear_outline = pngimg_init();
	canvas_ear_fill = pngimg_init();
	canvas_uppermane_outline = pngimg_init();
	canvas_uppermane_fill = pngimg_init();
	canvas_lowermane_outline = pngimg_init();
	canvas_lowermane_fill = pngimg_init();
	canvas_tail_outline = pngimg_init();
	canvas_tail_fill = pngimg_init();

	PNGIMG * canvas_socks_outline = pngimg_init();
	PNGIMG * canvas_socks_fill = pngimg_init();
	PNGIMG * canvas_socks_color1 = pngimg_init();
	PNGIMG * canvas_socks_color2 = pngimg_init();

	int skip = 0;
	if (pngimg_read(canvas_body_outline, "templates/body_outline.png") < 0) 
		skip = 1;
	if (pngimg_read(canvas_body_fill, "templates/body_fill.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_ear_outline, "templates/ear_outline.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_ear_fill, "templates/ear_fill.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_iris, "templates/eye_iris.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_iris_gradient, "templates/eye_irisgradient.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_irisline1, "templates/eye_irisline1.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_irisline2, "templates/eye_irisline2.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_pupil, "templates/eye_pupil.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_reflection, "templates/eye_reflection.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_brows, "templates/eye_brows.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_lashes, "templates/eye_lashes.png") < 0)
		skip = 1;
	if (pngimg_read(canvas_eye_sclera, "templates/eye_sclera.png") < 0)
		skip = 1;
	if (use_separated_eyes) {
		if (pngimg_read(canvas_reye_iris, "templates/reye_iris.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_iris_gradient, "templates/reye_irisgradient.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_irisline1, "templates/reye_irisline1.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_irisline2, "templates/reye_irisline2.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_pupil, "templates/reye_pupil.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_reflection, "templates/reye_reflection.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_reye_sclera, "templates/reye_sclera.png") < 0)
			skip = 1;
	}
	if (race & (PEGASUS | ALICORN)) {
		if (pngimg_read(canvas_wing_fill, "templates/wing_fill.png") < 0)
			skip = 1;
		if (pngimg_read(canvas_wing_outline, "templates/wing_outline.png") < 0)
			skip = 1;
	}
	if (use_socks) {
		if (pngimg_read(canvas_socks_outline, "templates/socks_outline.png") < 0)
			use_socks = 0;
		if (pngimg_read(canvas_socks_color1, "templates/socks_color1.png") < 0)
			use_socks = 0;
		if (pngimg_read(canvas_socks_color2, "templates/socks_color2.png") < 0)
			use_socks = 0;
		if (pngimg_read(canvas_socks_fill, "templates/socks_fill.png") < 0)
			use_socks = 0;
	}

	strtolower(uppermane);
	strtolower(lowermane);
	strtolower(tail);
	char um_loc[255];
	char lm_loc[255];
	char t_loc[255];
	char part_loc[255];
	um_loc[0] = 0;
	lm_loc[0] = 0;
	t_loc[0] = 0;
	part_loc[0] = 0;

	for (int i = 0; i < BODY_DETAIL_MAX; i++) {
		if (body_detail_s[i] == NULL) {
			continue;
		}
		sprintf(part_loc, "templates/detail/%s.png", body_detail_s[i]);
		canvas_body_detail[i] = pngimg_init();
		if (pngimg_read(canvas_body_detail[i], part_loc) < 0) {
			free(body_detail_s[i]);
			body_detail_s[i] = NULL;
		}
	}

	strcat(um_loc, "templates/");
	strcat(um_loc, uppermane);
	strcat(lm_loc, "templates/");
	strcat(lm_loc, lowermane);
	strcat(t_loc, "templates/");
	strcat(t_loc, tail);

	int part_len;
	char part_part[255];
	strcpy(part_loc,um_loc);
	part_len = strlen(part_loc);
	strcat(part_loc, "/uppermane_outline.png");
	if (pngimg_read(canvas_uppermane_outline, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	strcat(part_loc, "/uppermane_fill.png");
	if (pngimg_read(canvas_uppermane_fill, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	if (race & (UNICORN | ALICORN)) {
		strcat(part_loc, "/horn_fill.png");
		if (pngimg_read(canvas_horn_fill, part_loc) < 0)
			skip = 1;
		part_loc[part_len] = 0;
		strcat(part_loc, "/horn_outline.png");
		if (pngimg_read(canvas_horn_outline, part_loc) < 0)
			skip = 1;
		part_loc[part_len] = 0;
	}
	for (int i = 0; i < style_detail_count(uppermane); i++) {
		canvas_uppermane_detail[i] = pngimg_init();
		sprintf(part_part, "%s/uppermane_detail%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_uppermane_detail[i], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(uppermane); i++) {
		canvas_uppermane_color[i + 1] = pngimg_init();
		sprintf(part_part, "%s/uppermane_color%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_uppermane_color[i + 1], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(uppermane); i++) {
		canvas_uppermane_color_outline[i + 1] = pngimg_init();
		sprintf(part_part, "%s/uppermane_color%d_outline.png",part_loc,i + 1);
		if (pngimg_read(canvas_uppermane_color_outline[i + 1], part_part) < 0)
			skip = 1;
	}

	strcpy(part_loc,lm_loc);
	part_len = strlen(part_loc);
	strcat(part_loc, "/lowermane_outline.png");
	if (pngimg_read(canvas_lowermane_outline, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	strcat(part_loc, "/lowermane_fill.png");
	if (pngimg_read(canvas_lowermane_fill, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	for (int i = 0; i < style_detail_count(lowermane); i++) {
		canvas_lowermane_detail[i] = pngimg_init();
		sprintf(part_part, "%s/lowermane_detail%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_lowermane_detail[i], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(lowermane); i++) {
		canvas_lowermane_color[i + 1] = pngimg_init();
		sprintf(part_part, "%s/lowermane_color%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_lowermane_color[i + 1], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(lowermane); i++) {
		canvas_lowermane_color_outline[i + 1] = pngimg_init();
		sprintf(part_part, "%s/lowermane_color%d_outline.png",part_loc,i + 1);
		if (pngimg_read(canvas_lowermane_color_outline[i + 1], part_part) < 0)
			skip = 1;
	}

	strcpy(part_loc,t_loc);
	part_len = strlen(part_loc);
	strcat(part_loc, "/tail_outline.png");
	if (pngimg_read(canvas_tail_outline, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	strcat(part_loc, "/tail_fill.png");
	if (pngimg_read(canvas_tail_fill, part_loc) < 0)
		skip = 1;
	part_loc[part_len] = 0;
	for (int i = 0; i < style_detail_count(tail); i++) {
		canvas_tail_detail[i] = pngimg_init();
		sprintf(part_part, "%s/tail_detail%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_tail_detail[i], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(tail); i++) {
		canvas_tail_color[i + 1] = pngimg_init();
		sprintf(part_part, "%s/tail_color%d.png",part_loc,i + 1);
		if (pngimg_read(canvas_tail_color[i + 1], part_part) < 0)
			skip = 1;
	}
	for (int i = 0; i < style_color_count(tail); i++) {
		canvas_tail_color_outline[i + 1] = pngimg_init();
		sprintf(part_part, "%s/tail_color%d_outline.png",part_loc,i + 1);
		if (pngimg_read(canvas_tail_color_outline[i + 1], part_part) < 0)
			skip = 1;
	}


	if (!skip) {
		body_color.r += 128;
		body_color.g += 128;
		body_color.b += 128;
		body_color.a += 128;
		
		pngimg_colorify(canvas_body_outline, &body_color, 0.8);
		pngimg_colorify(canvas_body_fill, &body_color, 1.0);
		pngimg_colorify(canvas_ear_outline, &body_color, 0.8);
		pngimg_colorify(canvas_ear_fill, &body_color, 1.0);

		pngimg_colorify(canvas_uppermane_outline, &mane_color[0], 0.8);
		pngimg_colorify(canvas_uppermane_fill, &mane_color[0], 1.0);
		for (int i = 0; i < style_color_count(uppermane); i++) {
			pngimg_colorify(canvas_uppermane_color[i+1], &mane_color[i+1], 1.0);
			pngimg_colorify(canvas_uppermane_color_outline[i+1], &mane_color[i+1], 0.8);
		}
		for (int i = 0; i < style_detail_count(uppermane); i++)
			pngimg_colorify(canvas_uppermane_detail[i], &mane_detail[i], 1.0);
		if (race & (UNICORN | ALICORN)) {
			pngimg_colorify(canvas_horn_outline, &body_color, 0.8);
			pngimg_colorify(canvas_horn_fill, &body_color, 1.0);
		}

		pngimg_colorify(canvas_lowermane_outline, &mane_color[0], 0.8);
		pngimg_colorify(canvas_lowermane_fill, &mane_color[0], 1.0);
		for (int i = 0; i < style_color_count(lowermane); i++) {
			pngimg_colorify(canvas_lowermane_color[i+1], &mane_color[i+1], 1.0);
			pngimg_colorify(canvas_lowermane_color_outline[i+1], &mane_color[i+1], 0.8);
		}
		for (int i = 0; i < style_detail_count(lowermane); i++)
			pngimg_colorify(canvas_lowermane_detail[i], &mane_detail[i], 1.0);

		pngimg_colorify(canvas_tail_outline, &tail_color[0], 0.8);
		pngimg_colorify(canvas_tail_fill, &tail_color[0], 1.0);
		for (int i = 0; i < style_color_count(tail); i++) {
			pngimg_colorify(canvas_tail_color[i+1], &tail_color[i+1], 1.0);
			pngimg_colorify(canvas_tail_color_outline[i+1], &tail_color[i+1], 0.8);
		}
		for (int i = 0; i < style_detail_count(tail); i++)
			pngimg_colorify(canvas_tail_detail[i], &tail_detail[i], 1.0);

		which_eye = BOTH;
		if (use_separated_eyes) {
			which_eye = RIGHT;
			pngimg_colorify(canvas_reye_sclera, &eye_sclera[which_eye], 1.0);
			pngimg_colorify(canvas_reye_iris, &eye_iris[which_eye], 1.0);
			pngimg_colorify(canvas_reye_iris_gradient, &eye_iris_gradient[which_eye], 1.0);
			pngimg_colorify(canvas_reye_irisline1, &eye_irisline1[which_eye], 1.0);
			pngimg_colorify(canvas_reye_irisline2, &eye_irisline2[which_eye], 1.0);
			pngimg_colorify(canvas_reye_pupil, &eye_pupil[which_eye], 1.0);
			pngimg_colorify(canvas_reye_reflection, &eye_reflection[which_eye], 1.0);
			which_eye = LEFT;
		}
		pngimg_colorify(canvas_eye_sclera, &eye_sclera[which_eye], 1.0);
		pngimg_colorify(canvas_eye_iris, &eye_iris[which_eye], 1.0);
		pngimg_colorify(canvas_eye_iris_gradient, &eye_iris_gradient[which_eye], 1.0);
		pngimg_colorify(canvas_eye_irisline1, &eye_irisline1[which_eye], 1.0);
		pngimg_colorify(canvas_eye_irisline2, &eye_irisline2[which_eye], 1.0);
		pngimg_colorify(canvas_eye_pupil, &eye_pupil[which_eye], 1.0);
		pngimg_colorify(canvas_eye_reflection, &eye_reflection[which_eye], 1.0);
		pngimg_colorify(canvas_eye_lashes, &eye_lashes, 1.0);
		pngimg_colorify(canvas_eye_brows, &eye_brows, 1.0);

		pngimg_colorify(canvas_wing_outline, &body_color, 0.8);
		pngimg_colorify(canvas_wing_fill, &body_color, 1.0);

		for (int i = 0; i < BODY_DETAIL_MAX; i++) {
			if (body_detail_s[i] == NULL)
				continue;
			pngimg_colorify(canvas_body_detail[i], &body_detail[i], 1.0);
		}

		color black;
		color_init(&black);
		black.a = 255;
		switch (use_socks) {
			case SOCKS_NEW_MODEL:
				pngimg_colorify(canvas_socks_outline, &socks_new[0], 0.8);
				pngimg_colorify(canvas_socks_color1, &socks_new[1], 1.0);
				pngimg_colorify(canvas_socks_color2, &socks_new[2], 1.0);
				pngimg_colorify(canvas_socks_fill, &socks_new[0], 1.0);
				break;
			case SOCKS_MODEL:
				pngimg_colorify(canvas_socks_outline, &socks_model, 0.8);
				pngimg_colorify(canvas_socks_color1, &black, 1.0);
				pngimg_colorify(canvas_socks_color2, &socks_model, 0.8);
				pngimg_colorify(canvas_socks_fill, &socks_model, 1.0);
				break;
		}
		if (use_socks) {
			//puts("Colored socks");
			if (use_socks == SOCKS_NEW_MODEL) {
			}
			else if (use_socks == SOCKS_MODEL) {
			}
		}


		pngimg_merge(canvas, canvas_tail_fill);
		for (int i = 0; i < style_detail_count(tail); i++) {
			pngimg_merge(canvas, canvas_tail_detail[i]);
		}
		pngimg_merge(canvas, canvas_tail_outline);
		for (int i = 0; i < style_color_count(tail); i++) {
			pngimg_merge(canvas, canvas_tail_color[i + 1]);
			pngimg_merge(canvas, canvas_tail_color_outline[i + 1]);
		}
		pngimg_merge(canvas, canvas_body_fill);
		for (int i = 0; i < BODY_DETAIL_MAX; i++) {
			if (body_detail_s[i] == NULL)
				continue;
			pngimg_merge(canvas, canvas_body_detail[i]);
		}
		if (use_eyelashes)
			pngimg_merge(canvas, canvas_eye_lashes);
		pngimg_merge(canvas, canvas_eye_sclera);
		pngimg_merge(canvas, canvas_eye_iris);
		pngimg_merge(canvas, canvas_eye_iris_gradient);
		pngimg_merge(canvas, canvas_eye_irisline2);
		pngimg_merge(canvas, canvas_eye_irisline1);
		pngimg_merge(canvas, canvas_eye_pupil);
		pngimg_merge(canvas, canvas_eye_reflection);
		if (use_separated_eyes) {
			pngimg_merge(canvas, canvas_reye_sclera);
			pngimg_merge(canvas, canvas_reye_iris);
			pngimg_merge(canvas, canvas_reye_iris_gradient);
			pngimg_merge(canvas, canvas_reye_irisline2);
			pngimg_merge(canvas, canvas_reye_irisline1);
			pngimg_merge(canvas, canvas_reye_pupil);
			pngimg_merge(canvas, canvas_reye_reflection);
		}
		pngimg_merge(canvas, canvas_eye_brows);
		pngimg_merge(canvas, canvas_body_outline);
		if (use_socks) {
			//puts("Merged socks");
			pngimg_merge(canvas, canvas_socks_fill);
			pngimg_merge(canvas, canvas_socks_color1);
			pngimg_merge(canvas, canvas_socks_outline);
			pngimg_merge(canvas, canvas_socks_color2);
		}
		if (race & (PEGASUS | ALICORN)) {
			pngimg_merge(canvas, canvas_wing_fill);
			pngimg_merge(canvas, canvas_wing_outline);
		}
		pngimg_merge(canvas, canvas_lowermane_fill);
		for (int i = 0; i < style_detail_count(lowermane); i++) {
			pngimg_merge(canvas, canvas_lowermane_detail[i]);
		}
		pngimg_merge(canvas, canvas_lowermane_outline);
		for (int i = 0; i < style_color_count(lowermane); i++) {
			pngimg_merge(canvas, canvas_lowermane_color[i + 1]);
			pngimg_merge(canvas, canvas_lowermane_color_outline[i + 1]);
		}
		pngimg_merge(canvas, canvas_uppermane_fill);
		for (int i = 0; i < style_detail_count(uppermane); i++) {
			pngimg_merge(canvas, canvas_uppermane_detail[i]);
		}
		pngimg_merge(canvas, canvas_uppermane_outline);
		for (int i = 0; i < style_color_count(uppermane); i++) {
			pngimg_merge(canvas, canvas_uppermane_color[i + 1]);
			pngimg_merge(canvas, canvas_uppermane_color_outline[i + 1]);
		}
		if (race & (UNICORN | ALICORN)) {
			pngimg_merge(canvas, canvas_horn_fill);
			pngimg_merge(canvas, canvas_horn_outline);
		}
		pngimg_merge(canvas, canvas_ear_fill);
		pngimg_merge(canvas, canvas_ear_outline);//*/
	}

	strrchr(filename, '.')[0] = '\0';
	strcat(filename, ".png");
	//pngimg_merge(canvas_body_fill, canvas_body_outline);
	//pngimg_merge(canvas_body_outline, canvas_body_fill);
	//pngimg_merge(canvas_body_outline, canvas_ear_fill);
	//pngimg_merge(canvas_body_outline, canvas_ear_outline);
	//merge(top/target, bottom)
	pngimg_write(canvas, filename);

	close(fd);
}

void read_nbt_string(char * s, int fd) {
	uint8_t c;
	uint32_t l;
	l = 0;
	read(fd, &c, 1);
	read(fd, &l, 1);
	l |= c << 8;
	if (l > 256)
		l = 256;
	else if (l == 0) {
		s[0] = 0;
		return;
	}
	read(fd, s, l);
	s[l] = 0;
}

void strtolower(char * s) {
	for (int i = 0; s[i]; i++) {
		s[i] = tolower(s[i]);
	}
}

int sgetnum(char * s) {
	for (int i = 0; s[i]; i++) {
		if (isdigit(s[i]))
			return atoi(s + i);
	}
	return 0;
}

int style_detail_count(char * style) {
	if (!strcmp(style, "bookworm")) {
		return 3;
	}
	else if (!strcmp(style, "radical")) {
		return 1;
	}
	else if (!strcmp(style, "speedster")) {
		return 5;
	}
	return 0;
}

int style_color_count(char * style) {
	if (!strcmp(style, "bookworm")) {
		return 0;
	}
	else if (!strcmp(style, "adventurous")) {
		return 1;
	}
	else if (!strcmp(style, "filly")) {
		return 1;
	}
	else if (!strcmp(style, "showboat")) {
		return 1;
	}
	else if (!strcmp(style, "timid")) {
		return 1;
	}
	else if (!strcmp(style, "moon")) {
		return 1;
	}
	else if (!strcmp(style, "speedster")) {
		return 1;
	}
	return 0;
}
