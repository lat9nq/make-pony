/* pixel.cpp
 */

#include <stdio.h>
#include "pixel.h"
#include <math.h>
#include <float.h>

float max(float a, float b) {
	if (a > b)
		return a;
	return b;
}

float min(float a, float b) {
	if (a < b)
		return a;
	return b;
}

float pixel_v(Pixel * p) {
	return max(max(p->r,p->g),p->b);
}

float pixel_d(Pixel * p) {
	return pixel_v(p) - min(min(p->r,p->g),p->b);
}

float pixel_h(Pixel * p) {
	if (pixel_d(p) == 0)
		return 0.0f;
	float v = pixel_v(p);
	float ret = 0.0f;
	if (v == p->r)
		ret = (p->g - p->b)/(6.0f*pixel_d(p));
	else if (v == p->g)
		ret = (p->b - p->r)/(6.0f*pixel_d(p)) + 1.0f/3.0f;
	else if (v == p->b)
		ret = (p->r - p->g)/(6.0f*pixel_d(p)) + 2.0f/3.0f;
	ret -= (int)ret;
	return ret;
}

float pixel_s(Pixel *p) {
	if (pixel_v(p) == 0)
		return 0;
	return pixel_d(p)/pixel_v(p);
}

float pixel_sl(Pixel *p) {
	if (pixel_l(p) >= 255)
		return 0;
	return pixel_d(p)/(1.0f-fabs(2.0f*pixel_l(p)/255.0f-1.0f))*255.0f;
}

float pixel_l(Pixel * p) {
	return 0.299f * p->r + 0.587f * p->g + 0.114f * p->b;
}

float pixel_c(Pixel * p) {
	return (pixel_v(p)/255.0f)*pixel_s(p);
}

/*
float pixel_c(float v, float s) {
	return (v/255.0f)*s;
}*/

float mod(float x, float y) {
	while (x >= y)
		x -= y;
	while (x < 0.0f)
		x += y;
	return x;
}

void pixel_sethsv(Pixel * p, float hue, float sat, float val) {
	hue = mod(hue,255.0f);
	float hp = hue*6.0f/255.0f;
	float c = val / 255.0f * sat;
	float x = c*(1.0f-fabs(mod(hp,2.0f)-1.0f));
	val /= 255;
	if (1 >= hp) { p->r = c; p->g = x; p->b = 0; }
	else if (2 >= hp) { p->r = x; p->g = c; p->b = 0; }
	else if (3 >= hp) { p->r = 0; p->g = c; p->b = x; }
	else if (4 >= hp) { p->r = 0; p->g = x; p->b = c; }
	else if (5 >= hp) { p->r = x; p->g = 0; p->b = c; }
	else if (6 >= hp) { p->r = c; p->g = 0; p->b = x; }
	float m = val - c;
	//~ printf("%.02f");
	p->r = 255 * (p->r + m);
	p->g = 255 * (p->g + m);
	p->b = 255 * (p->b + m);
}

void pixel_seth(Pixel * p, float hue) {
	pixel_sethsv(p, hue, pixel_s(p), pixel_v(p));
}

void pixel_sets(Pixel * p, float sat) {
	pixel_sethsv(p, pixel_h(p)*255, sat, pixel_v(p));
}

void pixel_setl(Pixel * p, float lig) {
	float l = pixel_l(p);
	p->r *= lig / l;
	p->g *= lig / l;
	p->b *= lig / l;
}

void pizel_setv(Pixel * p, float val) {
	pixel_sethsv(p, pixel_h(p)*255, pixel_s(p), val);
}
