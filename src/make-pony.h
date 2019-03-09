#ifndef MAKE_PONY_H
#define MAKE_PONY_H

#include "nbt.h"

#define STYLECHARLEN	12
// #define TARGETCOUNT  	70
#define DETAILCHANCE  	15
#define DETAILCOUNT  	4

#define PI  	3.14159f
#define RAD(X)	PI * (X) / 180.0f

#define SECONDSPERDAY	86400

#define UMANE(X)	((X) + 0)
#define LMANE(X)	((X) + STYLECHARLEN)
#define TAIL(X) 	((X) + STYLECHARLEN*2)
#define COLOR(X)	*((X) + STYLECHARLEN*3)

#define STYLECOUNT  	16 

#define ADVENTUROUS	0
#define SHOWBOAT	1
#define BOOKWORM	2
#define SPEEDSTER	3
#define ASSERTIVE	4
#define RADICAL	5
#define BUMPKIN	6
#define FLOOFEH	7
#define INSTRUCTOR	8
#define FILLY	9
#define MOON	10
#define TIMID	11
#define BIGMAC	12
#define BOLD	13
#define MECHANIC	14
#define SHOWBOAT2	15

#define RACES	3
#define EARTH	0
#define PEGASUS	1
#define UNICORN	2

int at;
// #define PUSHTARGET(R,X,T)	sprintf((R)[at], "%c%s", (T), (X)); at++
#define PUSHSTRING(R,X)	strcpy((R)[at], (X)); at++
#define ADDSTYLE(R, I, S, T, U, C)	strcpy(UMANE((R)[(I)]), (S)); strcpy(LMANE((R)[(I)]), (T)); strcpy(TAIL((R)[(I)]), (U)); COLOR((R)[(I)]) = (C)
#define RIGHT(X,C)	((X)+strlen(X)-(C))
#define WARM(H)	((H) <= 120.0f || (H) > 300.0f)

int addString(char * key, char * data, int t, char * buffer);
int addValue(char * key, float data, int t, char * buffer);
int addColor(char * key, color * data, int t, char * buffer);
int addBool(char * key, int data, int t, char * buffer);
int addSeparator(char * key, char * buffer);

#endif
