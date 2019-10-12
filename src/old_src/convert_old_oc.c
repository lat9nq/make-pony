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
	char *s;
	s = (char *)malloc(sizeof(*s)*256);
	int i;
	while (1) {
		c = fgetc(f);
		if (feof(f))
			break;

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
		}
	}
	fclose(f);

	return 0;
}
