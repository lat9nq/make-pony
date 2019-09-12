/* pngimg.cpp
 */

#include <png.h>
#include "pngimg.h"
#include "pixel.h"
#include <stdlib.h>
#include <stdio.h>

PNGIMG::PNGIMG() {
	pixels = NULL;
	width = 0;
	height = 0;
}

PNGIMG::PNGIMG(int w, int h) {
	width = w;
	height = h;
	pixels = new Pixel*[height];
	for (int i = 0; i < height; i++) 
		pixels[i] = new Pixel[width];	
}

PNGIMG::~PNGIMG() {
	if (pixels != NULL) {
		for (int i = 0; i < height; i++) 
			delete pixels[i];
		delete pixels;
	}
	pixels = NULL;
	width = 0;
	height = 0;
}

png_byte ** PNGIMG::getData() {
	png_bytep * data = (png_bytep*)malloc(sizeof(*data)*height);
	for (int i = 0; i < height; i++) {
		data[i] = (png_byte*)malloc(sizeof(*data[i])*width*4);
		for (int j = 0; j < width; j++) {
			data[i][4*j+0] = pixels[i][j].r;
			data[i][4*j+1] = pixels[i][j].g;
			data[i][4*j+2] = pixels[i][j].b;
			data[i][4*j+3] = pixels[i][j].a;
		}
	}
	return data;
}

int PNGIMG::w() const {
	return width;
}

int PNGIMG::h() const {
	return height;
}

Pixel * PNGIMG::at(int x, int y) {
	return &pixels[y][x];
}

int PNGIMG::write(char * filename) {
	FILE * f = fopen(filename, "wb");
	png_bytep * rows = this->getData();
	if (f == NULL) {
		fprintf(stderr,"error: %s refused binary write access\n",filename);
		return -1;
	}
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure on write initialize\n");
		return -2;
	}
	png_init_io(png_ptr, f);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure writing header\n");
		return -3;
	}
	png_set_IHDR(png_ptr, info_ptr, width, height,
		8, 6, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure writing bytes\n");
		return -4;
	}
	png_write_image(png_ptr, rows);
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure finishing image\n");
		return -5;
	}
	png_write_end(png_ptr, NULL);
	
	for (int i = 0; i < height; i++) 
		free(rows[i]);
	free(rows);
	
	fclose(f);
	return 0;
}

/* Credits to Guillaume Cottenceau
 */
int PNGIMG::read(char * filename) {
	unsigned char header[8];
	
	FILE * f = fopen(filename, "rb");
	if (f == NULL) {
		fprintf(stderr, "error: %s refused read access\n", filename);
		return -1;
	}
	fread(header, 1, 8, f);
	if (png_sig_cmp(header, 0, 8)) {
		fprintf(stderr, "error: %s is not a well-formed PNG file\n", filename);
		return -2;
	}
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL){
		fprintf(stderr, "error: png_create_read_struct failed\n");
		return -3;
	}
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (png_ptr == NULL){
		fprintf(stderr, "error: png_create_info_struct failed\n");
		return -4;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure initializing i/o\n");
		return -5;
	}
	
	png_init_io(png_ptr, f);
	png_set_sig_bytes(png_ptr, 8);
	
	png_read_info(png_ptr, info_ptr);
	
	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	//~ int color_type = png_get_color_type(png_ptr, info_ptr);
	//~ int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	//~ printf("%d,%d\n",color_type, bit_depth);
	
	//~ number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);
	
	/* read file */
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr,"error: failure reading image\n");
		return -6;
	}

	png_bytep* data = (png_bytep*) malloc(sizeof(png_bytep) * height);
	pixels = new Pixel*[height];
	for (int y = 0; y < height; y++) {
		data[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
		pixels[y] = new Pixel[width];
	}
	png_read_image(png_ptr, data);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			pixels[y][x].r = (float)(data[y][4*x+0]);
			pixels[y][x].g = (float)(data[y][4*x+1]);
			pixels[y][x].b = (float)(data[y][4*x+2]);
			pixels[y][x].a = (float)(data[y][4*x+3]);
		}
		free(data[y]);
	}
	free(data);
	
	fclose(f);
	
	return 0;
}
