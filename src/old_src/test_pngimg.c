#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pngimg.h"

int main(int argc, char * argv[]) {
	PNGIMG * img;
	img = pngimg_init();
	pngimg_read(img, "5ce5a002.png");

	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			img->pixels[i][j].g = 0;
			img->pixels[i][j].b = 0;
		}
	}

	pngimg_write(img, "5ce5a002.2.png");

	return 0;
}
