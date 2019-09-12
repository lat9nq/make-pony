/* pngimg.h
 */


#ifndef PNGIMG_H
#define PNGIMG_H
#include <png.h>
#include "pixel.h"

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
};

#endif /* PNGIMG_H */ 
