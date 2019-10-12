/* pngimg.cpp
 */

#include <png.h>
#include "pngimg.h"
#include "pixel.h"
#include <stdlib.h>
#include <stdio.h>

PNGIMG * pngimg_init() {
	PNGIMG * img = (PNGIMG *)malloc(sizeof(*img));
	img->pixels = NULL;
	img->width = 0;
	img->height = 0;
	return img;
}

void pngimg_alloc(PNGIMG * img, int w, int h) {
	img->width = w;
	img->height = h;
	img->pixels = (Pixel**)malloc(sizeof(*(img->pixels)) * h);
	for (int i = 0; i < h; i++) 
		img->pixels[i] = (Pixel*)malloc(sizeof(*(img->pixels[i]))*w);
}

void pngimg_free(PNGIMG * img) {
	if (img->pixels != NULL) {
		for (int i = 0; i < img->height; i++) 
			free(img->pixels[i]);
		free(img->pixels);
	}
	img->pixels = NULL;
	img->width = 0;
	img->height = 0;
}

void pngimg_destroy_data(png_byte * data, void * _) {
	free(data);
}

void pngimg_destroy_datap(png_bytep * data) {
	for (int i = 0; data[i]; i++) {
		free(data[i]);
	}
	free(data);
}

png_byte ** pngimg_getData(PNGIMG * img) {
	png_bytep * data = (png_bytep*)malloc(sizeof(*data)*(img->height + 1));
	for (int i = 0; i < img->height; i++) {
		data[i] = (png_byte*)malloc(sizeof(*data[i])*img->width*4);
		for (int j = 0; j < img->width; j++) {
			data[i][4*j+0] = (uint8_t)img->pixels[i][j].r;
			data[i][4*j+1] = (uint8_t)img->pixels[i][j].g;
			data[i][4*j+2] = (uint8_t)img->pixels[i][j].b;
			data[i][4*j+3] = (uint8_t)img->pixels[i][j].a;
		}
		data[i + 1] = NULL;
	}
	return data;
}

png_byte * pngimg_get_data_array(PNGIMG * img) {
	int x;
	png_byte * data;
	
	data = (png_byte *)malloc(sizeof(*data)*img->height * img->width * 4);
	for (int i = 0; i < img->height; i++) {
		x = i * img->width;
		for (int j = 0; j < img->width; j++) {
			data[x + 0] = (uint8_t)img->pixels[i][j].r;
			data[x + 1] = (uint8_t)img->pixels[i][j].g;
			data[x + 2] = (uint8_t)img->pixels[i][j].b;
			data[x + 3] = (uint8_t)img->pixels[i][j].a;
			x += 4;
		}
	}
	return data;
}

int pngimg_width(PNGIMG * img) {
	return img->width;
}

int pngimg_height(PNGIMG * img) {
	return img->height;
}

Pixel * pngimg_at(PNGIMG * img, int x, int y) {
	return &(img->pixels[y][x]);
}

int pngimg_write(PNGIMG * img, FILE * f) {
	png_bytep * rows = pngimg_getData(img);
	if (f == NULL) {
		fprintf(stderr,"error: file refused binary write access\n");
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
	png_set_IHDR(png_ptr, info_ptr, img->width, img->height,
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
	
	for (int i = 0; i < img->height; i++) 
		free(rows[i]);
	free(rows);
	
	return 0;
}

int pngimg_read(PNGIMG * img, char * filename) {
	FILE * f;
	f = fopen(filename, "r");
	
	int val;
	val = pngimg_read_fp(img, f);
	
	fclose(f);
	
	return val;
}

/* Credits to Guillaume Cottenceau
 */
int pngimg_read_fp(PNGIMG * img, FILE * f) {
	unsigned char header[8];
	int width, height;
	
	if (f == NULL) {
		fprintf(stderr, "error: file refused read access\n");
		return -1;
	}
	fread(header, 1, 8, f);
	if (png_sig_cmp(header, 0, 8)) {
		fprintf(stderr, "error: file is not a well-formed PNG file\n");
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

	pngimg_free(img);
	pngimg_alloc(img, width, height);

	png_bytep* data = (png_bytep*) malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++) {
		data[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
	}
	png_read_image(png_ptr, data);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			img->pixels[y][x].r = (float)(data[y][4*x+0]);
			img->pixels[y][x].g = (float)(data[y][4*x+1]);
			img->pixels[y][x].b = (float)(data[y][4*x+2]);
			img->pixels[y][x].a = (float)(data[y][4*x+3]);
		}
		free(data[y]);
	}
	free(data);
	
	return 0;
}

int pngimg_merge(PNGIMG *img1, PNGIMG *img2) {
	if (img1->width != img2->width || img1->height != img2 -> height) {
		return -1;
	}
	for (int i = 0; i < img1->width; i++) {
		for (int j = 0; j < img1->height; j++) {
			Pixel * p2 = &img1->pixels[j][i];//pngimg_at(img1, j, i);
			Pixel * p1 = &img2->pixels[j][i];//pngimg_at(img2, j, i);
			if (p1->a == 0)
				continue;
			p2->r = (p1->r * p1->a + p2->r * p2->a * (255.0f - p1->a) / 255.0f)
				/(p1->a + p2->a * (255.0f - p1->a) / 255.0f);
			p2->g = (p1->g * p1->a + p2->g * p2->a * (255.0f - p1->a) / 255.0f)
				/(p1->a + p2->a * (255.0f - p1->a) / 255.0f);
			p2->b = (p1->b * p1->a + p2->b * p2->a * (255.0f - p1->a) / 255.0f)
				/(p1->a + p2->a * (255.0f - p1->a) / 255.0f);
			p2->a = p1->a + p2->a * (255.0f - p1->a) / 255.0f;

			if (p2->r > 255.0f)
				p2->r = 255.0f;
			if (p2->g > 255.0f)
				p2->g = 255.0f;
			if (p2->b > 255.0f)
				p2->b = 255.0f;

			if (p2->r < 0)
				p2->r = 0;
			if (p2->g < 0)
				p2->g = 0;
			if (p2->b < 0)
				p2->b = 0;
			if (p2->a > 255.0f)
				p2->a = 255.0f;
		}
	}
	return 0;
}

int pngimg_merge_and_free(PNGIMG *img1, PNGIMG *img2) {
	int val = pngimg_merge(img1, img2);
	pngimg_free(img2);
	return val;
}

void pngimg_colorify(PNGIMG *img, const color * c, float val) {
	color col;
	col.r = c->r * val;
	col.g = c->g * val;
	col.b = c->b * val;
	col.a = c->a;
	float alpha = c->a / 255.0f;
	Pixel * p;
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			p = &img->pixels[j][i];
			if (p->a == 0)
				continue;
			p->r = col.r;
			p->g = col.g;
			p->b = col.b;
			p->a *= alpha;
		}
	}
}

