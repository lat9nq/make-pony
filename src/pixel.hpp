/* pixel.h
 */

#ifndef PIXEL_H
#define PIXEL_H

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
};

#endif /* PIXEL_H */ 
