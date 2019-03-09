#ifndef NBT_H
#define NBT_H

#define NBT_BOOLEAN	0x01
#define NBT_SEPARATOR	0x04
#define NBT_FLOAT	0x05
#define NBT_COLOR	0x07
#define NBT_STRING	0x08
#define NBT_GROUP	0x0a

typedef union {
	float f;
	u_int8_t u[4];
	u_int32_t u32;
	u_int16_t u16[2];
} nbt_payload_t;

typedef struct {
	u_int8_t code;
	u_int16_t name_length;
	char name[256];
	void * payload;
} nbt_t;

#endif
