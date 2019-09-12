/* pixel.h
 */

#ifndef PIXEL_H
#define PIXEL_H

typedef struct Pixel_t {
	float r,g,b,a;
} Pixel;

float pixel_h(Pixel * p);
float pixel_s(Pixel * p);
float pixel_v(Pixel * p);
float pixel_l(Pixel * p);
void pixel_seth(Pixel * p, float h);
void pixel_sets(Pixel * p, float s);
void pixel_setv(Pixel * p, float v);
void pixel_setl(Pixel * p, float l);
float pixel_d(Pixel * p);
float pixel_c(Pixel * p);
float pixel_sl(Pixel * p);
void pixel_sethsv(Pixel * p, float h, float s, float v);

float max(float a, float b);
float min(float a, float b);
float mod(float x, float y);

/*
class Pixel {
	public:
		Pixel();
		~Pixel();
		float r,g,b,a;
		float h(); ///<hue
		float s(); ///<saturation
		float v(); ///<value
		float l(); ///<lightness
		void seth(float h);
		void sets(float s);
		void setl(float l);
		void setv(float v);
	private:
		float d(); ///<delta
		float c(); ///<chroma
		float c(float v, float s); ///<chroma
		float sl();
		void sethsv(float h, float s, float v);
		float max(float a, float b);
		float min(float a, float b);
		float mod(float x, float y);
}; */

#endif /* PIXEL_H */ 
