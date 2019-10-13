/* thumbnailer.c
 * 
 * A thumbnailer for Pony Player Models Second Generation (by dbotthepone).
 * 
 * Supports any hairstyle defined at style_{color,detail}_count, and any body
 * detail. Also supports socks (old and new models only).
 * 
 * Requires libpng, and its dependency zlib.
 */

#include "thumbnailer.h"

int thumbnail(PNGIMG * canvas, uint8_t * data, uint32_t data_len) {
	nbt_t nbt; // main NBT, using as a header for an array of NBTs
	int nbt_len, // max allocated length of the NBT data list
		nbt_at; // "where are we" in the NBT data
	nbt_len = 128;
	nbt_at = 0;

	/* Initialize the base NBT element */
	nbt.code = NBT_GROUP;
	nbt.name_length = 4;
	//strncpy(nbt.name, "data", nbt.name_length);
	memcpy(nbt.name, "data", nbt.name_length);
	nbt.payload = malloc(sizeof(nbt_t) * nbt_len);
	memset(nbt.payload, 0, sizeof(nbt_t) * nbt_len);

	color body_color;
	color body_detail[BODY_DETAIL_MAX]; // array of body detail colors
	color tail_color[6];
	color mane_color[6];
	color tail_detail[6];
	color mane_detail[6];
	uint8_t use_separated_eyes = 0; // boolean
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
	//while (read(fd, &c, 1)) {
	for (int i = 0; i < data_len; ) {
		c = data[i];
		i++;
		l = 0;
		s[0] = 0;
		switch (c) {
			case NBT_GROUP:
				i += get_nbt_string(s, data+i);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", GRUP);\n", s);
			break;
			case NBT_INT:
				i += get_nbt_string(s, data+i);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", INT);\n", s);
			case NBT_FLOAT:
				if (c == NBT_FLOAT)
					i += get_nbt_string(s, data+i);
				/*if (!strcmp(s, "tattoo_rotate_5")) {
					fprintf(stderr,"%s\n", s);
				}*/
				if (c == NBT_FLOAT && PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", VAL);\n", s);
				//fprintf(stderr,"%s\n", s);
				f.a[3] = data[i]; i++;
				f.a[2] = data[i]; i++;
				f.a[1] = data[i]; i++;
				f.a[0] = data[i]; i++;
			break;
			case NBT_STRING:
				i += get_nbt_string(s, data+i);
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
					fprintf(stderr,"%s\n", s);
					print_c = 1;
				}*/
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", STR);\n", s);
				i += get_nbt_string(s, data+i);
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
				i += get_nbt_string(s, data+i);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", BOOL);\n", s);
				c = data[i];
				i++;
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
				i += get_nbt_string(s, data+i);
				if (PRINT_TARGETS)
					printf("\tPUSHTARGET(target, \"%s\", COL);\n", s);
				l = *(int*)(data + i); i += 4;
				f = *(float_bin*)(data + i); i += 4;

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
					//fprintf(stderr,"%s\n", s);
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
						//fprintf(stderr,"%s %02x %02x %02x %02x\n", s, f.a[0], f.a[1], f.a[2], f.a[3]);
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
						//fprintf(stderr,"%02x %02x %02x %02s\n", f.a[0], f.a[1], f.a[2], f.a[3]);
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
						//fprintf(stderr,"%s %02x %02x %02x\n", s, tail_detail[sgetnum(s)-1].r, tail_detail[sgetnum(s)-1].g, tail_detail[sgetnum(s)-1].b);
					}
				}
				else if (strstr(s, "socks")) {
					if (strstr(s, "socks_new_model")) {
						socks_new[sgetnum(s)-1].r = f.a[0] + 128;
						socks_new[sgetnum(s)-1].g = f.a[1] + 128;
						socks_new[sgetnum(s)-1].b = f.a[2] + 128;
						socks_new[sgetnum(s)-1].a = 255;
						//fprintf(stderr,"%s %d %02x %02x %02x\n", s, sgetnum(s), f.a[0], f.a[1], f.a[2]);
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

	if (canvas->pixels == NULL)
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
		free(nbt_col);
	}

	PNGIMG ** pony_part;
	int pony_part_count;
	pony_part = (PNGIMG**)malloc(sizeof(*pony_part) * 1024);
	pony_part_count = 0;
	body_color.r += 128;
	body_color.g += 128;
	body_color.b += 128;
	body_color.a += 128;
	
	strtolower(uppermane);
	strtolower(lowermane);
	strtolower(tail);
	
	char um_loc[255];
	char lm_loc[255];
	char t_loc[255];
	char part_loc[512];
	um_loc[0] = 0;
	lm_loc[0] = 0;
	t_loc[0] = 0;
	part_loc[0] = 0;
	
	sprintf(um_loc, "templates/%s", uppermane);
	sprintf(lm_loc, "templates/%s", lowermane);
	sprintf(t_loc, "templates/%s", tail);

	sprintf(part_loc, "%s/tail_fill.png", t_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &tail_color[0], 1.0);
	sprintf(part_loc, "%s/tail_outline.png", t_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &tail_color[0], 0.8);
	
	for (int i = 0; i < style_detail_count(tail); i++) {
		sprintf(part_loc, "%s/tail_detail%d.png", t_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &tail_detail[i], 1.0);
	}
	for (int i = 0; i < style_color_count(tail); i++) {
		sprintf(part_loc, "%s/tail_color%d.png", t_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &tail_color[i+1], 1.0);
		
		sprintf(part_loc, "%s/tail_color%d_outline.png", t_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &tail_color[i+1], 0.8);
	}
	
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/body_fill.png", &body_color, 1.0);
	for (int i = 0; i < BODY_DETAIL_MAX; i++) {
		if (body_detail_s[i] == NULL || !strcmp(body_detail_s[i], "none")) {
			continue;
		}
		sprintf(part_loc, "templates/detail/%s.png", body_detail_s[i]);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &body_detail[i], 1.0);
	}
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/body_outline.png", &body_color, 0.8);
	
	which_eye = BOTH;
	if (use_separated_eyes) {
		which_eye = RIGHT;
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_sclera.png", &eye_sclera[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_iris.png", &eye_iris[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_irisgradient.png", &eye_iris_gradient[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_irisline1.png", &eye_irisline1[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_irisline2.png", &eye_irisline2[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_pupil.png", &eye_pupil[which_eye], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/reye_reflection.png", &eye_reflection[which_eye], 1.0);
		which_eye = LEFT;
	}
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_sclera.png", &eye_sclera[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_iris.png", &eye_iris[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_irisgradient.png", &eye_iris_gradient[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_irisline1.png", &eye_irisline1[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_irisline2.png", &eye_irisline2[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_pupil.png", &eye_pupil[which_eye], 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_reflection.png", &eye_reflection[which_eye], 1.0);
	
	if (use_eyelashes)
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_brows.png", &eye_lashes, 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/eye_lashes.png", &eye_brows, 1.0);
	
	if (use_socks == SOCKS_NEW_MODEL) {
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_fill.png", &socks_new[0], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_color1.png", &socks_new[1], 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_outline.png", &socks_new[0], 0.8);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_color2.png", &socks_new[2], 1.0);
	}
	else if (use_socks == SOCKS_MODEL) {
		color black;
		black.r = 0;
		black.g = 0;
		black.b = 0;
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_fill.png", &socks_model, 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_color1.png", &black, 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/socks_outline.png", &socks_model, 0.8);
	}
	
	if (race == PEGASUS || race == ALICORN) {
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/wing_fill.png", &body_color, 1.0);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/wing_outline.png", &body_color, 0.8);
	}

	sprintf(part_loc, "%s/lowermane_fill.png", lm_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[0], 1.0);
	sprintf(part_loc, "%s/lowermane_outline.png", lm_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[0], 0.8);
	for (int i = 0; i < style_color_count(lowermane); i++) {
		sprintf(part_loc, "%s/lowermane_color%d.png", lm_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[i+1], 1.0);
		
		sprintf(part_loc, "%s/lowermane_color%d_outline.png", lm_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[i+1], 0.8);
	}
	for (int i = 0; i < style_detail_count(lowermane); i++) {
		sprintf(part_loc, "%s/lowermane_detail%d.png", lm_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_detail[i], 1.0);
	}

	sprintf(part_loc, "%s/uppermane_fill.png", um_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[0], 1.0);
	sprintf(part_loc, "%s/uppermane_outline.png", um_loc);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[0], 0.8);
	for (int i = 0; i < style_color_count(uppermane); i++) {
		sprintf(part_loc, "%s/uppermane_color%d.png", um_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[i+1], 1.0);
		
		sprintf(part_loc, "%s/uppermane_color%d_outline.png", um_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_color[i+1], 0.8);
	}
	for (int i = 0; i < style_detail_count(uppermane); i++) {
		sprintf(part_loc, "%s/uppermane_detail%d.png", um_loc, i + 1);
		pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], part_loc, &mane_detail[i], 1.0);
	}
	
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/ear_fill.png", &body_color, 1.0);
	pony_part_count += thumbnail_add_part(&pony_part[pony_part_count], "templates/ear_outline.png", &body_color, 0.8);
	
	for (int i = 0; i < pony_part_count; i++) {
		pngimg_merge(canvas, pony_part[i]);
		pngimg_free(pony_part[i]);
		free(pony_part[i]);
	}
	free(pony_part);
	
	
	void * payload;
	for (int i = 0; i < nbt_at; i++) {
		payload = ((nbt_t *)(nbt.payload))[nbt_at].payload;
		if (payload != NULL) {
			free(payload);
		}
	}
	free(nbt.payload);
	for (int i = 0; i < BODY_DETAIL_MAX; i++) {
		if (body_detail_s[i] != NULL)
			free(body_detail_s[i]);
	}
	
	return 0;
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

int get_nbt_string(char * s, uint8_t * data) {
	uint8_t c;
	uint32_t l;
	int i = 0;
	l = 0;
	c = data[i];
	i++;
	l = data[i];
	i++;
	l |= c << 8;
	if (l > 256)
		l = 256;
	else if (l == 0) {
		s[0] = 0;
		return 2;
	}
	//printf("%d\n", l);
	for (int j = 0; j < l; j++)
		s[j] = (data + i)[j];
	i += l;
	s[l] = 0;
	
	return i;
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
		return 1;
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
	else if (!strcmp(style, "floofeh")) {
		return 1;
	}
	else if (!strcmp(style, "speedster")) {
		return 1;
	}
	else if (!strcmp(style, "mechanic")) {
		return 1;
	}
	return 0;
}

int thumbnail_add_part(PNGIMG ** img, char * filename, color * c, float value) {
	*img = pngimg_init();
	if (!pngimg_read(*img, filename)) {
		pngimg_colorify(*img, c, value);
		return 1;
	}
	free(*img);
	return 0;
}
