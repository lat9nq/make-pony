/* pixel.cpp
 */

#include <stdio.h>
#include "pixel.h"
#include <math.h>
#include <float.h>

uint8_t max(uint8_t a, uint8_t b) {
	if (a > b)
		return a;
	return b;
}

uint8_t min(uint8_t a, uint8_t b) {
	if (a < b)
		return a;
	return b;
}

uint8_t pixel_v(Pixel * p) {
	return max(max(p->r,p->g),p->b);
}

uint8_t pixel_d(Pixel * p) {
	return pixel_v(p) - min(min(p->r,p->g),p->b);
}

uint8_t pixel_h(Pixel * p) {
	if (pixel_d(p) == 0)
		return 0;
	uint8_t v = pixel_v(p);
	uint8_t ret = 0;
	if (v == p->r)
		ret = (p->g - p->b)/(6*pixel_d(p));
	else if (v == p->g)
		ret = (p->b - p->r)/(6*pixel_d(p)) + 1/3;
	else if (v == p->b)
		ret = (p->r - p->g)/(6*pixel_d(p)) + 2/3;
	ret -= (int)ret;
	return ret;
}

uint8_t pixel_s(Pixel *p) {
	if (pixel_v(p) == 0)
		return 0;
	return pixel_d(p)/pixel_v(p);
}

uint8_t pixel_sl(Pixel *p) {
	if (pixel_l(p) >= 255)
		return 0;
	return pixel_d(p)/(1-fabs(2*pixel_l(p)/255-1))*255;
}

uint8_t pixel_l(Pixel * p) {
	return 0.299f * p->r + 0.587f * p->g + 0.114f * p->b;
}

uint8_t pixel_c(Pixel * p) {
	return (pixel_v(p)/255)*pixel_s(p);
}

/*
uint8_t pixel_c(uint8_t v, uint8_t s) {
	return (v/255)*s;
}*/

uint8_t mod(uint8_t x, uint8_t y) {
	while (x >= y)
		x -= y;
	while (x < 0)
		x += y;
	return x;
}

void pixel_sethsv(Pixel * p, uint8_t hue, uint8_t sat, uint8_t val) {
	hue = mod(hue,255);
	uint8_t hp = hue*6/255;
	uint8_t c = val / 255 * sat;
	uint8_t x = c*(1-fabs(mod(hp,2)-1));
	val /= 255;
	if (1 >= hp) { p->r = c; p->g = x; p->b = 0; }
	else if (2 >= hp) { p->r = x; p->g = c; p->b = 0; }
	else if (3 >= hp) { p->r = 0; p->g = c; p->b = x; }
	else if (4 >= hp) { p->r = 0; p->g = x; p->b = c; }
	else if (5 >= hp) { p->r = x; p->g = 0; p->b = c; }
	else if (6 >= hp) { p->r = c; p->g = 0; p->b = x; }
	uint8_t m = val - c;
	//~ printf("%.02f");
	p->r = 255 * (p->r + m);
	p->g = 255 * (p->g + m);
	p->b = 255 * (p->b + m);
}

void pixel_seth(Pixel * p, uint8_t hue) {
	pixel_sethsv(p, hue, pixel_s(p), pixel_v(p));
}

void pixel_sets(Pixel * p, uint8_t sat) {
	pixel_sethsv(p, pixel_h(p)*255, sat, pixel_v(p));
}

void pixel_setl(Pixel * p, uint8_t lig) {
	uint8_t l = pixel_l(p);
	p->r *= lig / l;
	p->g *= lig / l;
	p->b *= lig / l;
}

void pizel_setv(Pixel * p, uint8_t val) {
	pixel_sethsv(p, pixel_h(p)*255, pixel_s(p), val);
}
