/* make-pony.c
 * C
 *
 * Toast Unlimited
 * 24 September 2017
 *
 * -s1506233186 -d1 is a known issue (completely black)
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "color.h"

#define STYLECHARLEN	12
#define TARGETCOUNT  	70
#define DETAILCHANCE  	15
#define DETAILCOUNT  	5

#define PI  	3.14159f
#define RAD(X)	PI * (X) / 180.0f

#define UMANE(X)	((X) + 0)
#define LMANE(X)	((X) + STYLECHARLEN)
#define TAIL(X) 	((X) + STYLECHARLEN*2)
#define COLOR(X)	*((X) + STYLECHARLEN*3)

#define STR 	1
#define COL 	2
#define VAL 	3
#define BOOL	4

#define STYLECOUNT  	16

#define ADVENTUROUS	0
#define SHOWBOAT	1
#define FILLY	2
#define BOOKWORM	3
#define SPEEDSTER	4
#define RADICAL	5
#define MOON	6
#define ASSERTIVE	7
#define TIMID	8
#define BIGMAC	9
#define BOLD	10
#define BUMPKIN	11
#define FLOOFEH	12
#define MECHANIC	13
#define SHOWBOAT2	14
#define INSTRUCTOR	15

#define RACES	3
#define EARTH	0
#define PEGASUS	1
#define UNICORN	2

int at;
#define PUSHTARGET(R,X,T)	sprintf((R)[at], "%c%s", (T), (X)); at++
#define PUSHSTRING(R,X)	strcpy((R)[at], (X)); at++
#define RIGHT(X,C)	((X)+strlen(X)-(C))
#define WARM(H)	((H) <= 120.0f || (H) > 300.0f)

void addString(char * key, char * data, int t, char * buffer);
void addValue(char * key, float data, int t, char * buffer);
void addColor(char * key, color * data, int t, char * buffer);
void addBool(char * key, int data, int t, char * buffer);

int main(int argc, char * argv[]) {
	
	char * filename;
	
	int times = 0;
	int fname_specified = 0;
	long int seed = time(NULL);
	int verbose = 0;
	int colorgiven = 0;
	int help = 0;
	int stdo = 0;
	
	color color1;
	int static_hue;
	hsv hsvcolor;
	int key = -1;
	int desaturated = -1;//!(rand() & 15);
	int white = -1;//desaturated && !(rand() & 3);
	// desaturated = desaturated ^ white;
	
	int male = 0;
	
	filename = malloc(sizeof(*filename)*256);
	
	if (argc != 1) {
		for (int i = 1; i < argc; i++) {
			if (argv[i][0] == '-') {
				switch (argv[i][1]) {
					case 's': //seed specify
						if (argv[i][2] != 0) {
							seed = atoi(argv[i]+2);
						}
						else {
							i++;
							seed = atoi(argv[i]);
						}
						// printf("%ld", seed);
					break;
					case 'k': //style _Key specify
						if (argv[i][2] != 0) {
							key = atoi(argv[i]+2);
						}
						else {
							i++;
							key = atoi(argv[i]);
						}
						key %= STYLECOUNT;
					break;
					case 'o': //output file
						if (argv[i][2] != 0) {
							strcpy(filename, argv[i]+2);
						}
						else {
							i++;
							strcpy(filename, argv[i]);
						}
						fname_specified = 1;
					break;
					case 'm': //how many?
						times = 1;
						if (argv[i][2] != 0) {
							times = atoi(argv[i]+2);
						}
						else {
							i++;
							times = atoi(argv[i]);
						}
					break;
					case 'c': 
						stdo = 1;
					break;
					case 'x': { //color specify
						char * arg;
						colorgiven = 1;
						if (argv[i][2] != 0) {
							arg = argv[i] + 2;
						}
						else {
							i++;
							arg = argv[i];
						}
						#pragma GCC diagnostic ignored "-Wformat"
						sscanf(arg, "%02X%02X%02X", &(color1.r), &(color1.g), &(color1.b));
						#pragma GCC diagnostic warning "-Wformat"
						static_hue = hue(&color1);
						hsvcolor.h = static_hue;
						hsvcolor.s = 1.0;
						hsvcolor.v = 1.0;
						hsvToRGB(&hsvcolor, &color1);
					}
					break;
					case 'd': //desaturate? 0 - no; 1 - yes; 2 - white
						if (argv[i][2]) {
							desaturated = atoi(argv[i]+2);
						}
						else {
							i++;
							if (i < argc && isdigit(argv[i][0])) {
								desaturated = atoi(argv[i]);
							}
							else {
								desaturated = 1;
							}
						}
						white = desaturated > 1;
						desaturated = (!!desaturated) ^ white;
					break;
					case 'v':
						verbose = 1;
					break;
					case 'g':
						male = 1;
					break;
					case 'h':
						help = 1;
					break;
					case '-': {
						if (strstr(argv[i], "--help")) {
							help = 1;
						}
					}
					break;
					default:
						help = 1;
					break;
				}
			}
		}
		//strcpy(filename, argv[1]);
	}
	
	if (help) {
		fprintf(stderr, "usage: %s [OPTIONS]\n", argv[0]);
		fprintf(stderr, "Creates a file compatible with Pony Player Models 2 addon for Garry's Mod 13.\n");
		fprintf(stderr, "By default, the output files take the format \"[SEED]_makepony.txt\",\n");
		fprintf(stderr, "and the default seed is the time since the UNIX epoch.\n");
		fprintf(stderr, "\t-o<file>\toutput to a specific file (incompatible with -m)\n");
		fprintf(stderr, "\t-m<n>\t\tgenerate n files\n\n");
		fprintf(stderr, "\t-d[0-2]\t\tdesaturate\n");
		fprintf(stderr, "\t\t\t0 -- use saturation\n");
		fprintf(stderr, "\t\t\t1 -- disallow saturation\n");
		fprintf(stderr, "\t\t\t2 -- use only white body colors\n");
		fprintf(stderr, "\t-g\t\tforce generate a male\n");
		fprintf(stderr, "\t-k<n>\t\tset a style to generate with (UMane, LMane, Tail)\n");
		fprintf(stderr, "\t\t\t0  -- ADVENTUROUS, ADVENTUROUS, ADVENTUROUS\n");
		fprintf(stderr, "\t\t\t1  -- SHOWBOAT, SHOWBOAT, SHOWBOAT\n");
		fprintf(stderr, "\t\t\t2  -- FILLY, ADVENTUROUS, SHOWBOAT\n");
		fprintf(stderr, "\t\t\t3  -- BOOKWORM, BOOKWORM, BOOKWORM\n");
		fprintf(stderr, "\t\t\t4  -- SPEEDSTER, SPEEDSTER, SPEEDSTER\n");
		fprintf(stderr, "\t\t\t5  -- RADICAL, NONE, RADICAL\n");
		fprintf(stderr, "\t\t\t6  -- ADVENTUROUS, MOON, SHOWBOAT\n");
		fprintf(stderr, "\t\t\t7  -- ASSERTIVE, ASSERTIVE, ASSERTIVE\n");
		fprintf(stderr, "\t\t\t8  -- BOOKWORM, TIMID, SHOWBOAT\n");
		fprintf(stderr, "\t\t\t9  -- ADVENTUROUS, ADVENTUROUS, RADICAL\n");
		fprintf(stderr, "\t\t\t10 -- BOLD, BOLD, RADICAL\n");
		fprintf(stderr, "\t\t\t11 -- BUMPKIN, BUMPKIN, BUMPKIN\n");
		fprintf(stderr, "\t\t\t12 -- FLOOFEH, FLOOFEH, FLOOFEH\n");
		fprintf(stderr, "\t\t\t13 -- MECHANIC, MOON, SPEEDSTER\n");
		fprintf(stderr, "\t\t\t14 -- SHOWBOAT, BOOKWORM, SHOWBOAT\n");
		fprintf(stderr, "\t\t\t15 -- INSTRUCTOR, NONE, RADICAL\n");
		fprintf(stderr, "\t-x<XXXXXX>\tspecify a base color to use in HEXADECIMAL\n\n");
		fprintf(stderr, "\t-s<n>\t\tspecify a seed for the random generator\n");
		fprintf(stderr, "\t-c\t\toutput to STDOUT\n\n");
		fprintf(stderr, "\t-v\t\tverbose output\n");
		fprintf(stderr, "\t-h, --help\tthis help text\n");
		fprintf(stderr, "written by Toast Unlimited<http://steamcommunity.com/id/toastunlimited/>\n");
		
		return 0;
	}
	
	if (times) {
		FILE * p;
		char * cmd = malloc(sizeof(*cmd)*256);
		char * temp = malloc(sizeof(*temp) * 128);
		
		for (int i = 0; i < times; i++) {
			cmd[0] = 0;
			temp[0] = 0;
			if (colorgiven) {
				//putchar('%');
				sprintf(temp, " -x%02X%02X%02X", color1.r, color1.g, color1.b);
			}
			if (desaturated != -1) {
				sprintf(temp, "%s -d%d", temp, desaturated + (white<<1));
			}
			if (male) {
				sprintf(temp, "%s -g", temp);
			}
			sprintf(cmd, "%s -s%ld %s %s" , argv[0], seed+i, (verbose) ? "-v" : "", temp);
			if (verbose) {
				fprintf(stderr, "%s\n", cmd);
			}
			p = popen(cmd, "r");
			fgets(cmd, 256, p);
			cmd[strlen(cmd)-1] = 0;
			if (!verbose) {
				printf("%ld: %s\n", seed + i, cmd);
			}
			else {
				putchar('\n');
			}
			pclose(p);
		}
		
		free(temp);
		free(cmd);
		return 0;
	}
	
	if (!fname_specified) {
		sprintf(filename, "%ld_makepony.txt", seed);
	}
	
	char ** styles;
	styles = malloc(sizeof(*styles)*STYLECOUNT);
	
	for (int i = 0; i < STYLECOUNT; i++) {
		styles[i] = calloc(STYLECHARLEN*3+1,sizeof(**styles));
	}
	
	strcpy(UMANE(styles[0]), "ADVENTUROUS");
	strcpy(LMANE(styles[0]), "ADVENTUROUS");
	strcpy(TAIL(styles[0]), "ADVENTUROUS");
	COLOR(styles[0]) = 2;
	
	strcpy(UMANE(styles[1]), "SHOWBOAT");
	strcpy(LMANE(styles[1]), "SHOWBOAT");
	strcpy(TAIL(styles[1]), "SHOWBOAT");
	COLOR(styles[1]) = 2;
	
	strcpy(UMANE(styles[2]), "FILLY");
	strcpy(LMANE(styles[2]), "ADVENTUROUS");
	strcpy(TAIL(styles[2]), "SHOWBOAT");
	COLOR(styles[2]) = 2;
	
	strcpy(UMANE(styles[3]), "BOOKWORM");
	strcpy(LMANE(styles[3]), "BOOKWORM");
	strcpy(TAIL(styles[3]), "BOOKWORM");
	COLOR(styles[3]) = 5;
	
	strcpy(UMANE(styles[4]), "SPEEDSTER");
	strcpy(LMANE(styles[4]), "SPEEDSTER");
	strcpy(TAIL(styles[4]), "SPEEDSTER");
	COLOR(styles[4]) = 7;
	
	strcpy(UMANE(styles[5]), "RADICAL");
	strcpy(LMANE(styles[5]), "NONE");
	strcpy(TAIL(styles[5]), "RADICAL");
	COLOR(styles[5]) = 7;
	
	strcpy(UMANE(styles[6]), "ADVENTUROUS");
	strcpy(LMANE(styles[6]), "MOON");
	strcpy(TAIL(styles[6]), "SHOWBOAT");
	COLOR(styles[6]) = 2;
	
	strcpy(UMANE(styles[7]), "ASSERTIVE");
	strcpy(LMANE(styles[7]), "ASSERTIVE");
	strcpy(TAIL(styles[7]), "ASSERTIVE");
	COLOR(styles[7]) = 2;
	
	strcpy(UMANE(styles[8]), "BOOKWORM");
	strcpy(LMANE(styles[8]), "TIMID");
	strcpy(TAIL(styles[8]), "SHOWBOAT");
	COLOR(styles[8]) = 5;
	
	strcpy(UMANE(styles[9]), "ADVENTUROUS");
	strcpy(LMANE(styles[9]), "ADVENTUROUS");
	strcpy(TAIL(styles[9]), "RADICAL");
	COLOR(styles[9]) = 2;
	
	strcpy(UMANE(styles[10]), "BOLD");
	strcpy(LMANE(styles[10]), "BOLD");
	strcpy(TAIL(styles[10]), "RADICAL");
	COLOR(styles[10]) = 3;
	
	strcpy(UMANE(styles[11]), "BUMPKIN");
	strcpy(LMANE(styles[11]), "BUMPKIN");
	strcpy(TAIL(styles[11]), "BUMPKIN");
	COLOR(styles[11]) = 3;
	
	strcpy(UMANE(styles[12]), "FLOOFEH");
	strcpy(LMANE(styles[12]), "FLOOFEH");
	strcpy(TAIL(styles[12]), "FLOOFEH");
	COLOR(styles[12]) = 2;
	
	strcpy(UMANE(styles[13]), "MECHANIC");
	strcpy(LMANE(styles[13]), "MOON");
	strcpy(TAIL(styles[13]), "SPEEDSTER");
	COLOR(styles[13]) = 7;
	
	strcpy(UMANE(styles[14]), "SHOWBOAT");
	strcpy(LMANE(styles[14]), "BOOKWORM");
	strcpy(TAIL(styles[14]), "SHOWBOAT");
	COLOR(styles[14]) = 5;
	
	strcpy(UMANE(styles[15]), "INSTRUCTOR");
	strcpy(LMANE(styles[15]), "NONE");
	strcpy(TAIL(styles[15]), "RADICAL");
	COLOR(styles[15]) = 2;
	
	srand(seed);
	
	int clrcount;
	if (key == -1) {
		key = rand() % STYLECOUNT;
	} else {
		rand();
	}
	clrcount = COLOR(styles[key]);
	
	if (!verbose && !stdo) {
		printf("%s\n", UMANE(styles[key]));
	}
	else {
		fprintf(stderr, "%ld: %s\n", seed, UMANE(styles[key]));
	}
	
	char ** target;
	target = malloc(sizeof(*target)*TARGETCOUNT);
	
	for (int i = 0; i < TARGETCOUNT; i++) {
		target[i] = malloc(sizeof(**target)*32);
	}
	
	at = 0;
	PUSHTARGET(target, "body", COL);
	PUSHTARGET(target, "mane_color_1", COL);
	PUSHTARGET(target, "mane_color_2", COL);
	PUSHTARGET(target, "mane_detail_color_1", COL);
	PUSHTARGET(target, "mane_detail_color_2", COL);
	PUSHTARGET(target, "mane_detail_color_3", COL);
	PUSHTARGET(target, "mane_detail_color_4", COL);
	PUSHTARGET(target, "mane_detail_color_5", COL);
	PUSHTARGET(target, "tail_color_1", COL);
	PUSHTARGET(target, "tail_color_2", COL);
	PUSHTARGET(target, "tail_detail_color_1", COL);
	PUSHTARGET(target, "tail_detail_color_2", COL);
	PUSHTARGET(target, "tail_detail_color_3", COL);
	PUSHTARGET(target, "tail_detail_color_4", COL);
	PUSHTARGET(target, "tail_detail_color_5", COL);
	PUSHTARGET(target, "horn_detail_color", COL);
	PUSHTARGET(target, "body_detail_1", STR);
	PUSHTARGET(target, "body_detail_2", STR);
	PUSHTARGET(target, "body_detail_3", STR);
	PUSHTARGET(target, "body_detail_4", STR);
	PUSHTARGET(target, "body_detail_5", STR);
	PUSHTARGET(target, "body_detail_6", STR);
	PUSHTARGET(target, "body_detail_7", STR);
	PUSHTARGET(target, "body_detail_8", STR);
	PUSHTARGET(target, "body_detail_color_1", COL);
	PUSHTARGET(target, "body_detail_color_2", COL);
	PUSHTARGET(target, "body_detail_color_3", COL);
	PUSHTARGET(target, "body_detail_color_4", COL);
	PUSHTARGET(target, "body_detail_color_5", COL);
	PUSHTARGET(target, "body_detail_color_6", COL);
	PUSHTARGET(target, "body_detail_color_7", COL);
	PUSHTARGET(target, "body_detail_color_8", COL);
	PUSHTARGET(target, "eye_iris1", COL);
	PUSHTARGET(target, "eye_iris2", COL);
	PUSHTARGET(target, "eye_irisline1", COL);
	PUSHTARGET(target, "eye_irisline2", COL);
	PUSHTARGET(target, "eye_reflection", COL);
	PUSHTARGET(target, "eye_effect", COL);
	PUSHTARGET(target, "eye_type", STR);
	PUSHTARGET(target, "eye_derp", BOOL);
	PUSHTARGET(target, "eye_lines", BOOL);
	PUSHTARGET(target, "cmark", BOOL);
	PUSHTARGET(target, "lips_color_inherit", BOOL);
	PUSHTARGET(target, "nose_color_inherit", BOOL);
	PUSHTARGET(target, "separate_horn", BOOL);
	PUSHTARGET(target, "socks_model", BOOL);
	PUSHTARGET(target, "socks", BOOL);
	PUSHTARGET(target, "socks_model_new", BOOL);
	PUSHTARGET(target, "socks_model", BOOL);
	PUSHTARGET(target, "gender", STR);
	PUSHTARGET(target, "suit", STR);
	PUSHTARGET(target, "mane_new", STR);
	PUSHTARGET(target, "manelower_new", STR);
	PUSHTARGET(target, "tail_new", STR);
	PUSHTARGET(target, "race", STR);
	PUSHTARGET(target, "eyelash", STR);
	PUSHTARGET(target, "body_lightwarp_texture", STR);
	PUSHTARGET(target, "weight", VAL);
	PUSHTARGET(target, "necksize", VAL);
	PUSHTARGET(target, "tailsize", VAL);
	PUSHTARGET(target, "legssize", VAL);
	PUSHTARGET(target, "ponysize", VAL);
	PUSHTARGET(target, "body_phong_boost", VAL); //0.09
	PUSHTARGET(target, "body_phong_middle", VAL); //5
	PUSHTARGET(target, "body_phong_exponent", VAL); //3
	PUSHTARGET(target, "body_phong_front", VAL); //1
	PUSHTARGET(target, "body_phong_sliding", VAL); //10
	PUSHTARGET(target, "body_phong_tint", COL);
	PUSHTARGET(target, "hoof_fluffers", BOOL);
	PUSHTARGET(target, "hoof_fluffers_strength", VAL);
	
	color * colors;
	colors = malloc(sizeof(*colors)*7);
	
	if (!colorgiven) {
		static_hue = rand() % 360;
		hsvcolor.h = static_hue;
		hsvcolor.s = 1.0f;
		hsvcolor.v = 1.0f;
		hsvToRGB(&hsvcolor, &color1);
	}
	else {
		rand();
	}
	
	male = ((static_hue % 180 < 60) * ((key == BIGMAC) || ((key == ADVENTUROUS) && (rand() & 3)))) || male;
	
	if (verbose) {
		fprintf(stderr, "color1: 0x%02X%02X%02X\n", color1.r, color1.g, color1.b);
		fprintf(stderr, "gender: %s\n", (male) ? "male" : "female");
	}
	
	// if (!(rand() & 7)) {
		// if (rand() & 0x1) {
			// static_hue = (static_hue + 150) % 360;
		// }
		// else {
			// static_hue = (static_hue + 210) % 360;
		// }
	// }
	
	float avg_sat = 0.0f;
	float std_dev = 0.0f;
	
	if (desaturated == -1) {
		desaturated = !(rand() & 15);
		white = desaturated && !(rand() & 3);
		desaturated = desaturated ^ white;
	}
	else {
		rand();
		rand();
	}
	int any_saturation = !desaturated;
	
	if (verbose) {
		fprintf(stderr, "body is %ssaturated\n", (desaturated) ? "de" : "");
	}
	
    for (int i = 0; i < clrcount; i++) {
		hsvcolor.h = static_hue;
		hsvcolor.s = 1;//saturation(&colors[0]);
		hsvcolor.v = 1;//value(&colors[0]);
		
		if (!(rand() & 15)) {
			if (rand() & 1) {//(WARM(static_hue)) {
				hsvcolor.h = (hsvcolor.h + 15) % 360;
			}
			else {
				hsvcolor.h = (hsvcolor.h + 345) % 360;
			}
		}
		if (key == BOOKWORM && i == 1) {
			colors[i] = colors[0];
		}
		else if (rand() & 1) {
			hsvcolor.s = (!desaturated) ? (rand() & 15) / 15.0f : 0.0f;
			if (desaturated) {
				hsvcolor.v = (!desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			}
		}
		else {
			hsvcolor.v = (!desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			any_saturation = (1 & (hsvcolor.v > 0.0f)) | any_saturation;
		}
		
		avg_sat += (!desaturated) * hsvcolor.s + (desaturated) * hsvcolor.v;
		hsvToRGB(&hsvcolor,&colors[i]);
		
		if (verbose) {
			fprintf(stderr, "\tcolors[%d]: 0x%02X%02X%02X\t HSV(%d,%g,%g)\n", i, colors[i].r, colors[i].g, colors[i].b, hsvcolor.h, hsvcolor.s, hsvcolor.v); 
		}
    }
	
	avg_sat /= (float)clrcount;
	
	for (int i = 0; i < clrcount; i++) {
		float x;
		x = (desaturated) * value(&colors[i]) + (!desaturated) * saturation(&colors[i]) - avg_sat;
		// if (verbose) {
			// fprintf(stderr, "x[%d] = %g\tvalue: %g\tsat: %g\n", i, x, value(&colors[i]), saturation(&colors[i]));
		// }
		x *= x;
		std_dev += x;
	}
	std_dev = sqrtf(std_dev);
	if (verbose) {
		fprintf(stderr,"avg_sat: %g\tstd_dev: %g\n", avg_sat, std_dev);
	}
	
	color bodycolor;
	if (!white) {
		#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
		hsvcolor.h = static_hue; // * (0.7 + 0.3 * lightness(&color1)))
		#pragma GCC diagnostic warning "-Wmaybe-uninitialized"
		// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * (0.4f + 0.6f * fmodf(fabs(-std_dev*1.5f + 1.25f), 1.0f))) : 0.0f;// * !(!(rand()&7));
		hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * (0.4f + std_dev * 0.6f)) : 0.0f;// * !(!(rand()&7));
		hsvcolor.v = (desaturated) ? (avg_sat * sqrtf(0.3f + std_dev*0.7f)) : 1.0f;//((rand() % 12) + 4) / 15.0f : 1.0f;
		// c = color1;
	}
	else {
		hsvcolor.h = 0;
		hsvcolor.s = 0;
		hsvcolor.v = 1;
	}
	hsvToRGB(&hsvcolor, &bodycolor);
	
	
	char ** races = malloc(sizeof(*races)*RACES);
	for (int i = 0; i < RACES; i++) {
		races[i] = malloc(sizeof(**races)*8);
	}
	at = 0;
	PUSHSTRING(races, "EARTH");
	PUSHSTRING(races, "PEGASUS");
	PUSHSTRING(races, "UNICORN");
	
	
	int details_in_use = 0;
	
	char ** details = malloc(sizeof(*details)*8);
	for (int i = 0; i < 8; i++) {
		details[i] = malloc(sizeof(**details)*16);
		details[i][0] = 0;
	}
	
	char ** available_details = malloc(sizeof(*available_details)*8);
	for (int i = 0; i < 8; i++) {
		available_details[i] = malloc(sizeof(**available_details)*16);
		available_details[i][0] = 0;
	}
	
	int at = 0;
	PUSHSTRING(available_details, "GRADIENT");
	PUSHSTRING(available_details, "HOOF_SMALL");
	PUSHSTRING(available_details, "STRIPES");
	PUSHSTRING(available_details, "LINES");
	PUSHSTRING(available_details, "FRECKLES");
	
	color * detail_color = malloc(sizeof(*detail_color)*8);
	for (int i = 0; i < 8; i++) {
		detail_color[i].r = 255;
		detail_color[i].g = 255;
		detail_color[i].b = 255;
	}
	
	int use_floofers = !(rand() % 6);
	if (use_floofers) {
		strcpy(details[details_in_use], "GRADIENT");
		if (any_saturation) {
			detail_color[details_in_use] = color1;
		}
		else {
			detail_color[details_in_use] = colors[0];
		}
		details_in_use += 1;
		strcpy(details[details_in_use], "HOOF_SMALL");
		detail_color[details_in_use] = colors[1 + (key == BOOKWORM || key == SPEEDSTER)];
		details_in_use += 1;
	}
	
	for (int i = 0 + 2 * use_floofers; i < DETAILCOUNT; i++) {
		if (!(rand() % DETAILCHANCE)) {
			strcpy(details[details_in_use], available_details[i]);
			detail_color[details_in_use] = colors[rand() % clrcount];
			if (verbose) {
				fprintf(stderr, "detail_color[%d]: 0x%02X%02X%02X\tdetails[%d]: %s\n", details_in_use, detail_color[details_in_use].r, detail_color[details_in_use].g, detail_color[details_in_use].b, details_in_use, details[details_in_use]);
			}
			details_in_use += 1;
		}
	}
	
	
	// -- BEGIN SYNTHESIZNIG DATA --
	
	char * data;
	data = malloc(sizeof(*data)*32768);
	if (!data) {
		fprintf(stderr, "error: could not allocate enough memory to store data\n");
		return 0;
	}
	
	char * s;
	s = malloc(sizeof(*s)*256);
	
	char * temp;
	temp = malloc(sizeof(*temp)*256);
	
	strcpy(data, "\n{\n");
	
	for (int i = 0; i < TARGETCOUNT; i++) {
		char * cur = target[i];
		s[0] = 0;
		if (cur[0] == COL) {
			color c;
			c = color1;
			
			if (!strcmp(cur+1,"body")) {
				// hsvcolor.h = static_hue;
				// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * sqrtf(0.5f + std_dev*0.5f) * (0.7 + 0.3 * lightness(&color1))) : 0.0f;// * !(!(rand()&7));
				// hsvcolor.v = (desaturated) ? (avg_sat * 0.8f * sqrtf(0.5f + std_dev*0.5f) * (0.7 + 0.3 * lightness(&color1))) : 1.0f;//((rand() % 12) + 4) / 15.0f : 1.0f;
				// // c = color1;
				// hsvToRGB(&hsvcolor, &c);
				c = bodycolor;
			}
			else if (strstr(cur+1, "body_detail")) {
				int num = atoi(RIGHT(cur,1));
				c = detail_color[num-1];
			}
			else if (strstr(cur+1, "_phong_")) {
				c.r = 1;
				c.g = 1;
				c.b = 1;
			}
			else if (strstr(cur+1, "horn") && !any_saturation) {
				c = colors[0];
			}
			else if (strstr(cur+1, "mane_") == cur + 1 || strstr(cur+1, "tail_") == cur + 1) {
				//printf("%s\n", RIGHT(cur+1,1));
				int num = 2 * (strstr(cur+1, "detail_") != NULL);
				num += atoi(RIGHT(cur,1));
				
				// if ((key == BIGMAC) && (!strcmp(cur+1, "tail_detail_color_1"))) {
					// num = 2;
				// }
				if ((key == BIGMAC || key == BUMPKIN || key == INSTRUCTOR || key == ASSERTIVE) && (strstr(cur+1, "_detail_color_"))) {
					num -= 1;
				}
				else if (key == INSTRUCTOR && strstr(cur+1, "mane_color_2")) {
					num -= 1;
				}
				
				c = colors[num-1];
				
				if (!strcmp(cur+1, "mane_color_2")) {
					if (key == BOOKWORM || key == ASSERTIVE || key == BUMPKIN) {
						c = colors[0];
					}
				}
				if (strstr(cur+1, "_color_2") && (key == TIMID) && !strstr(cur+1, "detail")) {
					c = colors[4];
				}
			}
			else if (strstr(cur+1, "eye_") == cur + 1) {
				if (strstr(cur+1, "effect")) {
					c.r = 0;
					c.g = 0;
					c.b = 0;
				}
				else if (strstr(cur+1, "reflection")) {
					c.r = 255;
					c.g = 255;
					c.b = 255;
				}
				else if (strstr(cur+1, "iris1") && !any_saturation) {
					hsvcolor.h = 0;
					hsvcolor.s = 0.0f;
					hsvcolor.v = 0.5f * (1.0f-value(&bodycolor));
					hsvToRGB(&hsvcolor, &c);
				}
				else if (strstr(cur+1, "iris2")) {
					c = bodycolor;
				}
				else if (strstr(cur+1, "irisline2")) {
					c = color1;
					hsvcolor.h = hue(&c);
					hsvcolor.s = 0.1f * any_saturation;
					hsvcolor.v = 0.9f + 0.1 * any_saturation;
					hsvToRGB(&hsvcolor, &c);
				}
				else if (strstr(cur+1, "irisline1")) {
					c = color1;
					hsvcolor.h = hue(&c);
					hsvcolor.s = 0.5f * any_saturation;
					hsvcolor.v = 0.5f + 0.5f * any_saturation;
					hsvToRGB(&hsvcolor, &c);
				}
			}
			
			addColor(cur+1, &c, 1, s);
		}
		else if (target[i][0] == STR) {
			strcpy(temp, "NONE");
			
			if (strstr(cur+1, "mane_") == cur + 1) {
				strcpy(temp, UMANE(styles[key]));
			}
			else if (strstr(cur+1, "manelower_") == cur + 1) {
				strcpy(temp, LMANE(styles[key]));
			}
			else if (strstr(cur+1, "tail_") == cur + 1) {
				strcpy(temp, TAIL(styles[key]));
			}
			else if (!strcmp(cur+1, "race")) {
				strcpy(temp, races[rand() % RACES]);
			}
			else if (strstr(cur+1, "body_detail")) {
				int num = atoi(RIGHT(cur,1));
				strcpy(temp, details[num-1]);
			}
			else if (!strcmp(cur+1, "gender")) {
				strcpy(temp, (male) ? "ADULT" : "FILLY");
			}
			else if (!strcmp(cur+1, "eyelash") && !male) {
				strcpy(temp, "DEFAULT");
			}
			else if (strstr(cur+1, "lightwarp")) {
				strcpy(temp, "SFM_PONY");
			}
			
			addString(cur+1, temp, 1, s);
		}
		else if (target[i][0] == BOOL) {
			int x = 0;
			
			x = (strstr(cur+1, "inherit") != NULL);
			x = (strstr(cur+1, "lines") != NULL) || x;
			x = (strstr(cur+1, "hoof_") && use_floofers) || x;
			
			addBool(cur+1, x, 1, s);
		}
		else if (target[i][0] == VAL) {
			float x = 1.0f;
			
			if (!strcmp(cur+1, "hoof_fluffers_strength")) {
				x = 0.8f;
			}
			else if (strstr(cur+1,"_phong_")) {
				if (strstr(cur+1,"_boost")) {
					x = 0.09f;
				}
				else if (strstr(cur+1,"_middle")) {
					x = 5.0f;
				}
				else if (strstr(cur+1,"_exponent")) {
					x = 3.0f;
				}
				else if (strstr(cur+1,"_front")) {
					x = 1.0f;
				}
				else if (strstr(cur+1,"_sliding")) {
					x = 10.0f;
				}
			}
			
			addValue(cur+1, x, 1, s);
		}
		strcat(data, s);
	}
	data[strlen(data)-2] = 0;
	strcat(data, "\n}\n");
	
	if (!stdo) {
		FILE * f = fopen(filename, "wb");
		if (!f) {
			fprintf(stderr, "error: output file not accessible\n");
			return 0;
		}
		fwrite(data, sizeof(*data), strlen(data), f);
		printf("Wrote to %s successfully\n", filename);
		fclose(f);
	}
	else {
		fprintf(stdout, "%s", data);
	}
	
	return 0;
}

void addString(char * key, char * data, int t, char * buffer) {
	char * tabs;
	tabs = malloc(sizeof(*tabs)*32);
	for (int i = 0; i < t; i++) {
		tabs[i] = '\t';
	}
	tabs[t] = 0;
	
	sprintf(buffer, "%s\"%s\": \"%s\",\n", tabs, key, data);
	free(tabs);
}

void addValue(char * key, float data, int t, char * buffer) {
	char * tabs;
	tabs = malloc(sizeof(*tabs)*32);
	for (int i = 0; i < t; i++) {
		tabs[i] = '\t';
	}
	tabs[t] = 0;
	
	sprintf(buffer, "%s\"%s\": %g,\n", tabs, key, data);
	free(tabs);
}

void addBool(char * key, int data, int t, char * buffer) {
	char * tabs;
	tabs = malloc(sizeof(*tabs)*32);
	for (int i = 0; i < t; i++) {
		tabs[i] = '\t';
	}
	tabs[t] = 0;
	
	sprintf(buffer, "%s\"%s\": %s,\n", tabs, key, (data ? "true" : "false"));
	free(tabs);
}

void addColor(char * key, color * data, int t, char * buffer) {
	char * tabs;
	tabs = malloc(sizeof(*tabs)*32);
	for (int i = 0; i < t; i++) {
		tabs[i] = '\t';
	}
	tabs[t] = 0;
	
	sprintf(buffer, "%s\"%s\": {\n%s\t\"r\": %d,\n%s\t\"g\": %d,\n%s\t\"b\": %d,\n%s\t\"a\": 255\n%s},\n", tabs, key, tabs, (*data).r, tabs, (*data).g, tabs, (*data).b, tabs, tabs);
	free(tabs);
}
