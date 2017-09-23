#ifndef COLOR_H
#define COLOR_H

typedef struct color_t {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} color;

typedef struct hsv_t {
	short h;
	float s;
	float v;
} hsv;

void hsvToRGB(const hsv * h, color * c);
int hue(const color * c);
float saturation(const color * c);
float value(const color * c);
float delta(const color * c);
int warm(const color * c);
float lightness(const color * c);
void analogous1(const color * c1, color * c2);
void analogous2(const color * c1, color * c2);

#endif