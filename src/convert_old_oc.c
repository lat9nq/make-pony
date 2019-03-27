#include "nbt.h"
#include "color.h"
#include "target.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("usage: %s [filename]\n", argv[0]);
		exit(0);
	}

	FILE * f;
	f = fopen(argv[1],"r");

	char c;
	char * s, *tag;
	s = (char *)malloc(sizeof(*s)*256);
	tag = (char *)malloc(sizeof(*tag)*256);
	int level,
	    mode,
	    read_string,
	    read_value,
	    line_number,
	    i,
	    assign,
	    type,
	    which_color,
	    alpha;
	double val;
	level
		= mode 
		= read_string 
		= read_value 
		= i 
		= assign 
		= which_color
		= 0;
	typedef enum {
		RED,
		GREEN,
		BLUE,
		ALPHA
	} colors;

	color clr;

	line_number = 1;
	while (!feof(f)) {
		c = fgetc(f);
		
		if (c == '{') {
			level++;
			assign = 0;
			type = NBT_COLOR;
		}
		else if (c == '}')
			level--;
		else if (c == ':')
			assign = 1;
		else if (c == '\n')
			line_number ++;



		if (read_string) {
			s[i] = c;
			i++;
		}
		if (c == '"') {
			if (i > 0)
				i--;
			s[i] = 0;
			read_string = !read_string;
			i = 0;

			if (!read_string && !assign) {
				if (level == 1)
					strcpy(tag, s);
				else
					switch s[0]:
						'r': which_color = RED;
							break;
						'g': which_color = GREEN;
							break;
						'b': which_color = BLUE;
							break;
						'a': which_color = ALPHA;
							break;
				printf("%d\t%s\n", line_number, s);
			}
			else if (!read_string && assign) {
				i = strlen(s);
			}
		}

		if ((isalnum(c) || c == '.') && !read_string && !read_value) {
			read_value = 1;
			alpha = 0;
			i = 0;
		}
		else if (!isalnum(c) && c != '.' && read_value) {
			s[i] = 0;
			read_value = 0;

			if (!assign) {
				printf("error: value without assign at line %d\n", line_number);
				exit(0);
			}

			if (alpha) {
				if (!strcmp(s,"true") || !strcmp(s,"false")) {
					type = NBT_BOOLEAN;
				}
				else
					printf("%s\n", s);
			}
			else {
				val = atof(s);
			}
		}
		if (read_value) {
			alpha = (isalpha(c) | alpha);
			s[i] = c;
			i++;
		}

		if (i && !read_value && !read_string) {
			assign = 0;
			if (level == 2) {
				switch which_color:
					RED:	clr.r = (int)val;
						break;
					GREEN: 	clr.g = (int)val;
						break;
					BLUE:	clr.b = (int)val;
						break;
					APLHA:	clr.a = (int)val;
						break;
			}
		}
	}

	return 0;
}
