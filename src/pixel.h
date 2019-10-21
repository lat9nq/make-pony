/* pixel.h
 */

#ifndef PIXEL_H
#define PIXEL_H

#include <stdlib.h>
#include <stdint.h>

typedef struct __attribute__((__packed__)) Pixel_t {
	uint8_t r,g,b,a;
} Pixel;

uint8_t pixel_h(Pixel * p);
uint8_t pixel_s(Pixel * p);
uint8_t pixel_v(Pixel * p);
uint8_t pixel_l(Pixel * p);
void pixel_seth(Pixel * p, uint8_t h);
void pixel_sets(Pixel * p, uint8_t s);
void pixel_setv(Pixel * p, uint8_t v);
void pixel_setl(Pixel * p, uint8_t l);
uint8_t pixel_d(Pixel * p);
uint8_t pixel_c(Pixel * p);
uint8_t pixel_sl(Pixel * p);
void pixel_sethsv(Pixel * p, uint8_t h, uint8_t s, uint8_t v);

uint8_t max(uint8_t a, uint8_t b);
uint8_t min(uint8_t a, uint8_t b);
uint8_t mod(uint8_t x, uint8_t y);

/*
class Pixel {
	public:
		Pixel();
		~Pixel();
		uint8_t r,g,b,a;
		uint8_t h(); ///<hue
		uint8_t s(); ///<saturation
		uint8_t v(); ///<value
		uint8_t l(); ///<lightness
		void seth(uint8_t h);
		void sets(uint8_t s);
		void setl(uint8_t l);
		void setv(uint8_t v);
	private:
		uint8_t d(); ///<delta
		uint8_t c(); ///<chroma
		uint8_t c(uint8_t v, uint8_t s); ///<chroma
		uint8_t sl();
		void sethsv(uint8_t h, uint8_t s, uint8_t v);
		uint8_t max(uint8_t a, uint8_t b);
		uint8_t min(uint8_t a, uint8_t b);
		uint8_t mod(uint8_t x, uint8_t y);
}; */

#endif /* PIXEL_H */ 
