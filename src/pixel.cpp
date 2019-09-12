/* pixel.cpp
 */

#include "stdio.h"
#include "pixel.h"
#include "math.h"
#include "float.h"

Pixel::Pixel() {
	r = 0;
	b = 0;
	g = 0;
	a = 0;
}

Pixel::~Pixel() {
	
}

float Pixel::max(float a, float b) {
	if (a > b)
		return a;
	return b;
}

float Pixel::min(float a, float b) {
	if (a < b)
		return a;
	return b;
}

float Pixel::v() {
	return max(max(r,g),b);
}

float Pixel::d() {
	return v() - min(min(r,g),b);
}

float Pixel::h() {
	if (d() == 0)
		return 0.0f;
	float v = this->v();
	float ret = 0.0f;
	if (v == r)
		ret = (g - b)/(6.0f*d());
	else if (v == g)
		ret = (b - r)/(6.0f*d()) + 1.0f/3.0f;
	else if (v == b)
		ret = (r - g)/(6.0f*d()) + 2.0f/3.0f;
	ret -= (int)ret;
	return ret;
}

float Pixel::s() {
	if (v() == 0)
		return 0;
	return d()/v();
}

float Pixel::sl() {
	if (l() >= 255)
		return 0;
	return d()/(1.0f-fabs(2.0f*l()/255.0f-1.0f))*255.0f;
}

float Pixel::l() {
	return 0.299f * r + 0.587f * g + 0.114f * b;
}

float Pixel::c() {
	return (v()/255.0f)*s();
}

float Pixel::c(float v, float s) {
	return (v/255.0f)*s;
}

float Pixel::mod(float x, float y) {
	while (x >= y)
		x -= y;
	while (x < 0.0f)
		x += y;
	return x;
}

void Pixel::sethsv(float hue, float sat, float val) {
	hue = mod(hue,255.0f);
	float hp = hue*6.0f/255.0f;
	float x = c(val,sat)*(1.0f-fabs(mod(hp,2.0f)-1.0f));
	float c = this->c(val, sat);
	val /= 255;
	if (1 >= hp) { r = c; g = x; b = 0; }
	else if (2 >= hp) { r = x; g = c; b = 0; }
	else if (3 >= hp) { r = 0; g = c; b = x; }
	else if (4 >= hp) { r = 0; g = x; b = c; }
	else if (5 >= hp) { r = x; g = 0; b = c; }
	else if (6 >= hp) { r = c; g = 0; b = x; }
	float m = val - c;
	//~ printf("%.02f");
	r = 255 * (r + m);
	g = 255 * (g + m);
	b = 255 * (b + m);
}

void Pixel::seth(float hue) {
	sethsv(hue, s(), v());
}

void Pixel::sets(float sat) {
	sethsv(h()*255, sat, v());
}

void Pixel::setl(float lig) {
	float l = this->l();
	r *= lig / l;
	g *= lig / l;
	b *= lig / l;
}

void Pixel::setv(float val) {
	sethsv(h()*255, s(), val);
}
