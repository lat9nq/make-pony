#ifndef NBT_H
#define NBT_H

#include <stdlib.h>

#define NBT_BOOLEAN	0x01
#define NBT_SEPARATOR	0x04
#define NBT_FLOAT	0x05
#define NBT_COLOR	0x07
#define NBT_STRING	0x08
#define NBT_GROUP	0x0a

typedef union {
	float f;
	unsigned char u[4];
	unsigned int u32;
	unsigned short u16[2];
} nbt_payload_t;

typedef struct {
	unsigned char code;
	unsigned short name_length;
	char name[256];
	void * payload;
} nbt_t;

#endif
