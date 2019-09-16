#include "color.h"
#include <math.h>
#include <stdlib.h>

#define max(X, Y)	((X) > (Y) ? (X) : (Y))
#define min(X, Y)	((X) < (Y) ? (X) : (Y))

void color_init(color * c) {
	c->r = 0;
	c->g = 0;
	c->b = 0;
	c->a = 0;
}

void hsvToRGB(const hsv * hclr, color * clr) {
	float h = (*hclr).h;
	float s = (*hclr).s;
	float v = (*hclr).v;
	float c = v * s;
	float x = c * (1.0f-fabs(fmodf(h/60.0f,2) - 1.0f));
	float m = v - c;
	c *= 255;
	x *= 255;
	m *= 255;
	if (h < 60) {
		(*clr).r = c;
		(*clr).g = x;
		(*clr).b = 0;
	}
	else if (h < 120) {
		(*clr).r = x;
		(*clr).g = c;
		(*clr).b = 0;
	}
	else if (h < 180) {
		(*clr).r = 0;
		(*clr).g = c;
		(*clr).b = x;
	}
	else if (h < 240) {
		(*clr).r = 0;
		(*clr).g = x;
		(*clr).b = c;
	}
	else if (h < 300) {
		(*clr).r = x;
		(*clr).g = 0;
		(*clr).b = c;
	}
	else {// if (h < 360) {
		(*clr).r = c;
		(*clr).g = 0;
		(*clr).b = x;
	}
	(*clr).r += m;
	(*clr).g += m;
	(*clr).b += m;
}

int hue(const color * c) {
	float r = (*c).r/255.0f;
	float g = (*c).g/255.0f;
	float b = (*c).b/255.0f;
	float v = value(c);
	float d = delta(c);
	float h = 0.0f;
	if (d == 0) {
		return 0;
	}
	else if (v == r) {
		h = (g - b) / (6.0f * d);
		if (h < 0.0f) {
			h += 1.0f;
		}
	}
	else if (v == g) {
		h = (b - r) / (6.0f * d) + 1.0f/3.0f;
	}
	else if (v == b) {
		h = (r - g) / (6.0f * d) + 2.0f/3.0f;
	}
	return (int)(h * 360.0f);
}

float saturation(const color * c) {
	float v = value(c);
	if (v != 0) {
		return delta(c) / value(c);
	}
	return 0;
}

float value(const color * c) {
	return max(max((*c).r, (*c).g), (*c).b) / 255.0f;
}

float delta(const color * c) {
	return value(c) - min(min((*c).r, (*c).g), (*c).b) / 255.0f;
}

int warm(const color * c) {
	float h = hue(c);
	return (h <= 120.0f) && (h > 300.0f);
}

float lightness(const color * c) {
	// return ((*c).r * 0.299f + (*c).g * 0.587f + (*c).b * 0.114f) / 255.0f;
	return 0.3f * ((*c).r + (*c).g + (*c).b) / 255.0f;
}

void analogous1(const color * c1, color * c2) {
}

void analogous2(const color * c1, color * c2) {
}
