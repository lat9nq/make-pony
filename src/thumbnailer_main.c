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

#include "thumbnailer.h"

int main(int argc, char * argv[]) {
	/* Point to the filename */
	const char * filename;
	filename = "thumb.dat";
	int fd;
	/* Open a file descriptor to the file if specified */
	if (argc == 2) {
		filename = argv[1];
#ifdef _WIN64
		/* In Windows, we must open it as a binary or risk an early file close */
		fd = open(filename, O_RDONLY | O_BINARY);
#else
		fd = open(filename, O_RDONLY);
#endif
	}
	else {
		fd = STDIN_FILENO;
	}

	/* If we can't read the file, exit */
	if (fd == -1) {
		fprintf(stderr,"error: %s isn't accessible\n", filename);
		return 0;
	}
	
	PNGIMG * canvas;
	canvas = pngimg_init();
	
	uint8_t c;
	uint8_t * data;
	data = malloc(sizeof(*data) * 32768);
	memset(data, 0, 32768);
	int data_len;
	data_len = 0;
	while (read(fd, &c, 1)) {
		data[data_len] = c;
		data_len++;
	}
	close(fd);
	data[data_len] = 0;
	
	thumbnail(canvas, data, data_len);

	FILE * outf;
	if (argc > 1) {
		char name[255];
		strcpy(name, filename);
		strrchr(name, '.')[0] = '\0';
		strcat(name, ".png");
		outf = fopen(name, "w");
	}
	else {
		outf = fdopen(STDOUT_FILENO, "w");
	}
	pngimg_write(canvas, outf);
	fclose(outf);
	
	return 0;
}
