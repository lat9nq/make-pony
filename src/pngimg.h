/* pngimg.h
 */


#ifndef PNGIMG_H
#define PNGIMG_H
#include <png.h>
#include <stdint.h>
#include "pixel.h"
#include "color.h"

typedef struct PNGIMG_t {
	Pixel ** pixels;
	int width, height;
} PNGIMG;

PNGIMG * pngimg_init();
void pngimg_alloc(PNGIMG * img, int w, int h);
void pngimg_free(PNGIMG * img);
png_byte ** pngimg_getData(PNGIMG * img);
int pngimg_width(PNGIMG * img);
int pngimg_height(PNGIMG * img);
Pixel * pngimg_at(PNGIMG * img, int x, int y);
int pngimg_read(PNGIMG * img, char * filename);
int pngimg_write(PNGIMG * img, char * filename);

int pngimg_merge(PNGIMG *img1, PNGIMG *img2);
void pngimg_colorify(PNGIMG *img1, const color * c, float val);

/* Old C++ things
class PNGIMG {
	public:
		PNGIMG();
		PNGIMG(int w, int h);
		~PNGIMG();
		png_byte ** getData();
		int w() const;
		int h() const;
		Pixel * at(int x, int y);
		int read(char * filename);
		int write(char * filename);
	private:
		Pixel ** pixels;
		int width, height;
}; */

#endif /* PNGIMG_H */ 
