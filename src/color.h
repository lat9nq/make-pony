#ifndef COLOR_H
#define COLOR_H
#include <stdlib.h>
#include <stdint.h>

typedef struct color_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} color;

typedef struct hsv_t {
	short h;
	float s;
	float v;
} hsv;

void color_init(color * c);
void hsvToRGB(const hsv * h, color * c);
int hue(const color * c);
float saturation(const color * c);
float value(const color * c);
float delta(const color * c);
int warm(const color * c);
float lightness(const color * c);

#endif
