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
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include "color.h"
#include "target.h"
#include "make-pony.h"

int main(int argc, char * argv[]) {

	char * filename;

	int times = 0;
	int fname_specified = 0;
	long int seed = time(NULL);
	int verbose = 0;
	int colorgiven = 0;
	int help = 0;
	int stdo = 0;
	int traditional = 0;

	char * farg = NULL;
	FILE * arg_in = NULL;
	char ** fargv = NULL;

	color color1;
	int static_hue = 0;
	int hair_hue;
	hsv hsvcolor;
	int key = -1;
	int desaturated = -1;//!(rand() & 15);
	int white = -1;//desaturated && !(rand() & 3);
	int wheel = -1;
	int separate_hair = 0;
	// desaturated = desaturated ^ white;

	int male = 0;

	filename = malloc(sizeof(*filename)*256);

	char * s;
	int s_len;
	s_len = 0;
	s = malloc(sizeof(*s)*256);
	s[0] = 0;

	char ** args = argv;
	int argsc = argc;

	if (argsc != 1) {
		for (int i = 1; i < argsc; i++) {
			if (args[i][0] == '-') {
				switch (args[i][1]) {
					case 's': //seed specify
						if (args[i][2] != 0) {
							seed = atoi(args[i]+2);
						}
						else {
							i++;
							seed = atoi(args[i]);
						}
						// printf("%ld", seed);
						break;
					case 'k': //style _Key specify
						if (args[i][2] != 0) {
							key = atoi(args[i]+2);
						}
						else {
							i++;
							key = atoi(args[i]);
						}
						key %= STYLECOUNT;
						break;
					case 'o': //output file
						if (args[i][2] != 0) {
							strcpy(filename, args[i]+2);
						}
						else {
							i++;
							strcpy(filename, args[i]);
						}
						fname_specified = 1;
						break;
					case 'm': //how many?
						times = 1;
						if (args[i][2] != 0) {
							times = atoi(args[i]+2);
						}
						else {
							i++;
							times = atoi(args[i]);
						}
						break;
					case 'c': 
						stdo = 1;
						break;
					case 'x': { //color specify
							  char * arg;
							  colorgiven = 1;
							  if (args[i][2] != 0) {
								  arg = args[i] + 2;
							  }
							  else {
								  i++;
								  arg = args[i];
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
						  if (args[i][2]) {
							  desaturated = atoi(args[i]+2);
						  }
						  else {
							  i++;
							  if (i < argsc && isdigit(args[i][0])) {
								  desaturated = atoi(args[i]);
							  }
							  else {
								  desaturated = 1;
							  }
						  }
						  white = desaturated > 1;
						  desaturated = (!!desaturated) ^ white;
						  break;
					case 'w':
						  if (args[i][2]) {
							  wheel = atoi(args[i]+2);
						  }
						  else {
							  i++;
							  if (i < argsc && isdigit(args[i][0])) {
								  wheel = atoi(args[i]);
							  }
						  }
						  break;
					case 'v':
						  verbose = 1;
						  break;
					case 'g':
						  male = 1;
						  break;
					case 'r':
						  separate_hair = 1;
						  break;
					case 't':
						  traditional = 1;
						  break;
					case 'h':
						  help = 1;
						  break;
					case '-': {
							  if (strstr(args[i], "--help")) {
								  help = 1;
							  }
						  }
						  break;
					default:
						  help = 1;
						  break;
				}
			}
			else {
				if (!farg) {
					farg = args[i];
					arg_in = fopen(farg, "r");
					fargv = malloc(sizeof(*fargv)*32);
					int j;
					for (j = 0; j < 32; j++) {
						fargv[j] = NULL;
					}
					j = 0;
					while (fgets(s,256,arg_in)) {
						fargv[j] = malloc(sizeof(**fargv)*256);
						strcpy(fargv[j], s);
						printf("%s",fargv[j]);
						j++;
					}
					args = fargv;
					i = -1;
					argsc = j;
				}
				else {
					fprintf(stderr, "error: specified too many input files\n");
					return 0;
				}
			}
		}
		//strcpy(filename, args[1]);
	}

	if (help) {
		fprintf(stderr, "usage: %s [OPTIONS]\n", argv[0]);
		fprintf(stderr, "Creates a file compatible with Pony Player Models 2 addon for Garry's Mod 13.\n");
		fprintf(stderr, "By default, the output files take the format \"[TIME]_[SEED]_makepony.dat\",\n");
		fprintf(stderr, "and the default seed is the time since the UNIX epoch.\n");
		fprintf(stderr, "\t-o<file>\toutput to a specific file (incompatible with -m)\n");
		fprintf(stderr, "\t-m<n>\t\tgenerate n files\n\n");
		fprintf(stderr, "\t-w\t\tcolor wheel options [15/56 chance per color]\n");
		fprintf(stderr, "\t\t\t0 -- do not use\n");
		fprintf(stderr, "\t\t\t1 -- use analogous colors [default]\n");
		fprintf(stderr, "\t\t\t2 -- use rectangular/tetradic colors\n");
		fprintf(stderr, "\t-t\t\ttraditional -- turns off body details\n");
		fprintf(stderr, "\t-r\t\tseparate hair from body color\n");
		fprintf(stderr, "\t-d[0-2]\t\tdesaturate\n");
		fprintf(stderr, "\t\t\t0 -- use saturation\n");
		fprintf(stderr, "\t\t\t1 -- disallow saturation\n");
		fprintf(stderr, "\t\t\t2 -- use only white body colors\n");
		fprintf(stderr, "\t-g\t\tforce generate a male\n");
		fprintf(stderr, "\t-k<n>\t\tset a style to generate with (UMane, LMane, Tail)\n");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 0, "ADVENTUROUS", "ADVENTUROUS", "ADVENTUROUS");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 1, "SHOWBOAT", "SHOWBOAT", "SHOWBOAT");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 2, "BOOKWORM", "BOOKWORM", "BOOKWORM");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 3, "SPEEDSTER", "SPEEDSTER", "SPEEDSTER");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 4, "ASSERTIVE", "ASSERTIVE", "ASSERTIVE");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 5, "RADICAL", "NONE", "RADICAL");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 6, "BUMPKIN", "BUMPKIN", "BUMPKIN");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 7, "FLOOFEH", "FLOOFEH", "FLOOFEH");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 8, "INSTRUCTOR", "NONE", "RADICAL");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 9, "FILLY", "ADVENTUROUS", "SHOWBOAT");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 10, "ADVENTUROUS", "MOON", "SHOWBOAT");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 11, "BOOKWORM", "TIMID", "SHOWBOAT");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 12, "ADVENTUROUS", "ADVENTUROUS", "RADICAL");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 13, "BOLD", "BOLD", "RADICAL");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 14, "MECHANIC", "MOON", "ADVENTUROUS");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 15, "SHOWBOAT", "BOOKWORM", "SHOWBOAT");
		fprintf(stderr, "\t\t\t%d -- %s, %s, %s\n", 16, "MAILCALL", "MAILCALL", "MAILCALL");
		fprintf(stderr, "\t-x<XXXXXX>\tspecify a base color to use in HEXADECIMAL\n\n");
		fprintf(stderr, "\t-s<n>\t\tspecify a seed for the random generator\n");
		fprintf(stderr, "\t-c\t\toutput to STDOUT\n\n");
		fprintf(stderr, "\t-v\t\tverbose output\n");
		fprintf(stderr, "\t-h, --help\tthis help text\n");
		fprintf(stderr, "written by Toast Unlimited<http://steamcommunity.com/id/toastunlimited/>\n");

		return 0;
	}
	long the_time = time(NULL);

	srand(seed);

	if (times) {
		FILE * p;
		char * cmd = malloc(sizeof(*cmd)*256);
		char * temp = malloc(sizeof(*temp) * 128);
		temp[0] = 0;
		for (int i = 1 - (argv != args); i < argsc; i++) {
			if (args[i][1] != 'm' && args[i][1] != 's' && args[i][1] != 'o') {
				strcat(temp, " ");
				strcat(temp, args[i]);
			}
			else {
				if (args[i][2] == 0) {
					i++;
				}
			}
		}

		printf("The seed is: %ld\n", seed);

		for (int i = 0; i < times; i++) {
			cmd[0] = 0;
			seed = rand() * rand() & 2147483647;
			sprintf(cmd, "%s -s%ld%s -o%03d_%010lu_makepony.dat" , argv[0], seed, temp, i, seed);
			if (verbose) {
				fprintf(stderr, "%s\n", cmd);
			}
			p = popen(cmd, "r");
			if (!fgets(cmd, 256, p))
				break;
			cmd[strlen(cmd)-1] = 0;
			if (!verbose) {
				printf("%03d_%010lu: %s\n", i, seed, cmd);
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
		sprintf(filename, "%09ld_%ld_makepony.dat", the_time, seed);
	}

#ifdef _WIN64
	strcpy(s, filename);
	strcpy(filename, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\GarrysMod\\garrysmod\\data\\ppm2\\");
	strcat(filename, s);
#else
#ifdef _WIN32
	strcpy(s, filename);
	strcpy(filename, "C:\\Program Files\\Steam\\steamapps\\common\\GarrysMod\\garrysmod\\data\\ppm2\\");
	// strcpy(filename, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\GarrysMod\\garrysmod\\data\\ppm2\\");
	strcat(filename, s);
#endif
#endif

	s[0] = 0;


	char ** styles;
	styles = malloc(sizeof(*styles)*STYLECOUNT);

	for (int i = 0; i < STYLECOUNT; i++) {
		styles[i] = calloc(STYLECHARLEN*3+1,sizeof(**styles));
	}

	// #define ADDSTYLE(R, I, S, T, U, C)	strcpy(UMANE((R)[(I)]), (S)); strcpy(LMANE((R)[(I)]), (T)); strcpy(TAIL((R)[(I)]), (U)); COLOR((R)[(I)]) = (C)
	ADDSTYLE(styles, 0, "ADVENTUROUS", "ADVENTUROUS", "ADVENTUROUS", 2);
	ADDSTYLE(styles, 1, "SHOWBOAT", "SHOWBOAT", "SHOWBOAT", 2);
	ADDSTYLE(styles, 2, "BOOKWORM", "BOOKWORM", "BOOKWORM", 5);
	ADDSTYLE(styles, 3, "SPEEDSTER", "SPEEDSTER", "SPEEDSTER", 7);
	ADDSTYLE(styles, 4, "ASSERTIVE", "ASSERTIVE", "ASSERTIVE", 2);
	ADDSTYLE(styles, 5, "RADICAL", "NONE", "RADICAL", 7);
	ADDSTYLE(styles, 6, "BUMPKIN", "BUMPKIN", "BUMPKIN", 3);
	ADDSTYLE(styles, 7, "FLOOFEH", "FLOOFEH", "FLOOFEH", 2);
	ADDSTYLE(styles, 8, "INSTRUCTOR", "NONE", "RADICAL", 2);
	ADDSTYLE(styles, 9, "FILLY", "ADVENTUROUS", "SHOWBOAT", 2);
	ADDSTYLE(styles, 10, "ADVENTUROUS", "MOON", "SHOWBOAT", 2);
	ADDSTYLE(styles, 11, "BOOKWORM", "TIMID", "SHOWBOAT", 5);
	ADDSTYLE(styles, 12, "ADVENTUROUS", "ADVENTUROUS", "RADICAL", 2);
	ADDSTYLE(styles, 13, "BOLD", "BOLD", "RADICAL", 3);
	ADDSTYLE(styles, 14, "MECHANIC", "MOON", "ADVENTUROUS", 2);
	ADDSTYLE(styles, 15, "SHOWBOAT", "BOOKWORM", "SHOWBOAT", 5);
	ADDSTYLE(styles, 16, "MAILCALL", "MAILCALL", "MAILCALL", 1);

	int clrcount;
	if (key == -1) {
		key = rand() % STYLECOUNT;
		if (traditional) {
			key %= 9;
		}
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
	importTargets(target);


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

	hair_hue = static_hue;
	/* 	if (separate_hair) {
		hair_hue = rand() % 360;
		} */

	male = ((static_hue % 180 < 60) && ((key == BIGMAC) || ((key == ADVENTUROUS) && (rand() & 3)))) || male;

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
		desaturated = !(rand() & 15) ;
		white = desaturated && !(rand() & 3);
		desaturated = desaturated ^ white;
		if (traditional) {
			desaturated = 0;
		}
	}
	else {
		rand();
		rand();
	}
	int any_saturation = !desaturated;

	if (verbose) {
		fprintf(stderr, "body is %ssaturated\n", (desaturated) ? "de" : "");
	}

	int r;
	int r2;

	for (int i = 0; i < clrcount; i++) { //get hair colors
		hsvcolor.h = static_hue;
		hsvcolor.s = 1;//saturation(&colors[0]);
		hsvcolor.v = 1;//value(&colors[0]);

		r = rand();
		r2 = rand();

		if (wheel) {
			if (wheel == 1 || wheel == -1) { // analogous (1/4 if set, 1/8 if not)
				if (!(r & 7)) {
					hsvcolor.h += 360/12;
				}
				else if (!((r & 7) - 1)) {
					hsvcolor.h += 360*11/12;
				}
			}
			else if (wheel == 2) { // rectangular
				if (r2 & 1) {
					if (!(r & 7)) {
						hsvcolor.h += 360/12*2;
					}
					/*else if (!((r & 7) - 1)) {
						hsvcolor.h += 8*360/12;
					}*/
				}
				else { // reverse rectangular
					if (!((r & 7) - 1)) {
						hsvcolor.h += 4*360/12;
					}
					/*else if (!(r & 7)) {
						hsvcolor.h += 10*360/12;
					}*/
				}
			}
			hsvcolor.h %= 360;
		}

		if (separate_hair && (r & 7)) {
			float del = ((r % 100) + (r2 % 100)) / 200.0f;
			del = del * 2.0f - 1.0f;
			if (verbose) {
				fprintf(stderr, "\t\tdel: %g\n", del);
			}
			hsvcolor.h = (int)(static_hue + 30 * del + 360) % 360;
		}
		if (key == BOOKWORM && i == 1) {
			colors[i] = colors[0];
		}
		else if (r2 & (1/*  + 2 * (hsvcolor.h != static_hue) */)) {
			hsvcolor.s = (!desaturated) ? (rand() & 15) / 15.0f : 0.0f;
			if (desaturated) {
				hsvcolor.v = (!desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			}
		}
		else {
			hsvcolor.v = (!desaturated) ? sqrtf((rand() % 12 + 4) / 15.0f) : sqrtf((rand() & 15) / 15.0f);
			any_saturation = (1 & (hsvcolor.v > 0.0f)) | any_saturation;
		}

		avg_sat += ((!desaturated) * hsvcolor.s + (desaturated) * hsvcolor.v) / pow(2,i);
		hsvToRGB(&hsvcolor,&colors[i]);

		if (verbose) {
			fprintf(stderr, "\tcolors[%d]: 0x%02X%02X%02X\t HSV(%d,%g,%g)\n", i, colors[i].r, colors[i].g, colors[i].b, hsvcolor.h, hsvcolor.s, hsvcolor.v); 
		}
	}

	// avg_sat /= (float)clrcount;
	avg_sat /= 2.0f;

	for (int i = 0; i < clrcount; i++) {
		float x;
		// x = (desaturated) * value(&colors[i]) + (!desaturated) * saturation(&colors[i]) - avg_sat;
		x = (((!desaturated) * saturation(&colors[i]) + (desaturated) * value(&colors[i])) / pow(2,i)) - avg_sat / pow(2,i);
		if (verbose) {
			fprintf(stderr, "x[%d] = %g\tvalue: %g\tsat: %g\n", i, x, value(&colors[i]), saturation(&colors[i]));
		}
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
		// hsvcolor.s = (!desaturated) ? (avg_sat * 0.8f * (0.4f + std_dev * 0.6f)) : 0.0f;// * !(!(rand()&7));
		hsvcolor.s = (!desaturated) ? (avg_sat * sqrt(std_dev)) : 0.0f;// * !(!(rand()&7));
		hsvcolor.v = (desaturated) ? (avg_sat * sqrtf(0.3f + std_dev*0.7f)) : 1.0f;//((rand() % 12) + 4) / 15.0f : 1.0f;
		// c = color1;
		// if (!desaturated) {
		// hsvToRGB(&hsvcolor, &bodycolor);
		// float l = lightness(&bodycolor);
		// l *= l;
		// hsvcolor.s = avg_sat - 0.3f*l;
		// }
		hsvcolor.s = ((float)(hsvcolor.s >= 0)) * hsvcolor.s;
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
		strcpy(details[i], "NONE");
	}

	char ** available_details = malloc(sizeof(*available_details)*8);
	for (int i = 0; i < 8; i++) {
		available_details[i] = malloc(sizeof(**available_details)*16);
		available_details[i][0] = 0;
	}

	color * detail_color = malloc(sizeof(*detail_color)*8);
	for (int i = 0; i < 8; i++) {
		detail_color[i].r = 255;
		detail_color[i].g = 255;
		detail_color[i].b = 255;
	}
	int use_floofers = (!(rand() % 6)) * (!traditional);

	if (!traditional) {
		at = 0;
		PUSHSTRING(available_details, "GRADIENT");
		// PUSHSTRING(available_details, "HOOF_SMALL");
		PUSHSTRING(available_details, "STRIPES");
		PUSHSTRING(available_details, "LINES");
		PUSHSTRING(available_details, "FRECKLES");

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
	}


	// -- BEGIN SYNTHESIZNIG DATA --

	unsigned char * data;
	data = malloc(sizeof(*data)*32768);
	if (!data) {
		fprintf(stderr, "error: could not allocate enough memory to store data\n");
		return 0;
	}
	memset(data, 0, sizeof(*data)*32768);
	int data_len;
	data_len = 0;

	char * temp;
	temp = malloc(sizeof(*temp)*256);

	//strcpy(data, "\n{\n");
	*(data) = NBT_GROUP;
	data_len ++;
	*(data + data_len) = 0x00;
	data_len++;
	*(data + data_len) = 0x04;
	data_len++;
	strcpy((char *)(data+data_len), "data");
	data_len += strlen("data");

	for (int i = 0; i < TARGETCOUNT; i++) {
		char * cur = target[i];
		s[0] = 0;
		if (cur[0] == SEP) {
			s_len = addSeparator(cur+1, s);
			putchar('x');
		}
		else if (cur[0] == COL) {
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

				hsvcolor.h = (static_hue - hue(&c) + hair_hue) % 360;
				hsvcolor.s = saturation(&c);
				hsvcolor.v = value(&c);
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "eye_") == cur + 1) {
				if (strstr(cur+1, "effect") || strstr(cur+1, "hole")) {
					c.r = 0;
					c.g = 0;
					c.b = 0;
				}
				// else if (strstr(cur+1, "reflection")) {
				// c.r = 255;
				// c.g = 255;
				// c.b = 255;
				// }
				else if (strstr(cur+1, "iris1")) {
					if (!any_saturation) {
						hsvcolor.h = 0;
						hsvcolor.s = 0.0f;
						hsvcolor.v = 0.5f * (1.0f-value(&bodycolor));
						hsvToRGB(&hsvcolor, &c);
					}
					else {
						c = color1;
					}
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
				else {
					c.r = 255;
					c.g = 255;
					c.b = 255;
				}
			}
			else if (strstr(cur+1, "mouth_")) {
				hsvcolor.h = 327;
				hsvcolor.s = 0.39f;
				hsvcolor.v = 1;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "tongue_")) {
				/*hsvcolor.h = (static_hue + 45 ) % 360;
				*/
				hsvcolor.h = 27;
				hsvcolor.s = 1;
				hsvcolor.v = 1;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "eyelash") || strstr(cur + 1, "eyebrow")) {
				hsvcolor.h = 0;
				hsvcolor.s = 0;
				hsvcolor.v = 0;
				hsvToRGB(&hsvcolor, &c);
			}
			else if (strstr(cur+1, "teeth")) {
				c.r = 255;
				c.g = 255;
				c.b = 255;
			}

			s_len = addColor(cur+1, &c, 1, s);
		}
		else if (target[i][0] == STR) {
			strcpy(temp, "NONE");

			if (strstr(cur+1, "url")) {
				temp[0] = 0;
			}
			else if (strstr(cur+1, "mane_") == cur + 1) {
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

			s_len = addString(cur+1, temp, 1, s);
		}
		else if (target[i][0] == BOOL) {
			int x = 0;

			x = (strstr(cur+1, "inherit") != NULL);
			x = (strstr(cur+1, "lines") != NULL) || x;
			x = (strstr(cur+1, "hoof_") && use_floofers) || x;
			x = (strstr(cur+1, "weapon_hide")) || x;
			x = (strstr(cur+1, "new_male_muzzle")) || x;

			s_len = addBool(cur+1, x, 1, s);
		}
		else if (target[i][0] == VAL || target[i][0] == INT) {
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
			else if (strstr(cur+1, "_wing_")) {
				x = 0.0f;
				if (strstr(cur+1, "_size")) {
					x = 1.0f;
				}
			}
			else if (strstr(cur+1, "eye") && !strstr(cur+1, "hole")) {
				x = 1;
				if (strstr(cur + 1, "rotation")) {
					x = 0;
				}
			}
			else if (strstr(cur+1, "hole")) {
				x = 0.75f;
				if (strstr(cur+1, "shift")) {
					x = 0;
				}
				else if (strstr(cur+1, "width") || strstr(cur+1, "height")) {
					x = 1;
				}
			}
			else if (strstr(cur+1, "rotat")) {
				x = 0;
			}

			if (target[i][0] == VAL)
				s_len = addValue(cur+1, x, 1, s);
			else
				s_len = addInt(cur+1, (int)x, 1, s);
		}
		memcpy(data + data_len, s, s_len);
		data_len += s_len;
	}
	//data[strlen(data)-2] = 0;
	//strcat(data, "\n}\n");
	*(data + data_len) = 0x00;
	data_len++;
	*(data + data_len) = 0x0a;
	data_len++;

	if (!stdo) {
		FILE * f = fopen(filename, "wb");
#ifdef _WIN32
		if (!f) {
			char * oldfilename = filename;
			filename = strrchr(filename, '\\')+1;
			fprintf(stderr, "warning: could not write to %s, trying %s\n", oldfilename, filename);
			f = fopen(filename, "wb");
		}
#endif
		if (!f) {
			fprintf(stderr, "error: output file not accessible\n");
			return 0;
		}
		fwrite(data, sizeof(*data), data_len, f);
		printf("Wrote to %s successfully\n", filename);
		fclose(f);
	}
	else {
		fprintf(stdout, "%s", data);
	}

	return 0;
}
