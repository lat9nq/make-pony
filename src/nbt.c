#include "nbt.h"

nbt_t * nbt_read() {
	nbt_t * nbt;
	int nbt_len;

	nbt_len = 64;
	nbt = (nbt_t *)malloc(sizeof(*nbt)*nbt_len);

	return NULL;
}

int addString(char * key, char * data, int t, char * buffer) {
	int i;
	int length;
	i = 0;
	length = strlen(key);
	t = strlen(data);
	//printf("%04x (%d) %s\n", t, t, key);

	*(buffer) = NBT_STRING;
	i++;

	*(buffer + i) = ((uint8_t *)(&length))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[0];
	i++;
	strcpy(buffer + i, key);
	i += strlen(key);

	*(buffer + i) = ((uint8_t *)(&t))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&t))[0];
	i++;
	strcpy(buffer + i, data);
	i += strlen(data);

	return i;
}

int addValue(char * key, float data, char * buffer) {
	int i, length;
	i = 0;
	length = strlen(key);

	*(buffer) = NBT_FLOAT;
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[0];
	i++;
	strcpy(buffer + i, key);
	i += strlen(key);

	int j;
	for (j = 0; j < 4; j++) {
		*(buffer + i) = ((uint8_t *)(&data))[3 - j];
		i++;
	}

	return i;
}

int addBool(char * key, int data, char * buffer) {
	int i, length;
	i = 0;
	length = strlen(key);

	*(buffer) = NBT_BOOLEAN;
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[0];
	i++;
	strcpy(buffer + i, key);
	i += strlen(key);

	if (data)
		*(buffer + i) = 1;
	else
		*(buffer + i) = 0;
	i++;

	return i;
}

int addColor(char * key, color * data, char * buffer) {
	int i, length;
	i = 0;
	length = strlen(key);

	*(buffer) = NBT_COLOR;
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[0];
	i++;
	strcpy(buffer + i, key);
	i += strlen(key);

	*(buffer + i) = 0;
	i++;
	*(buffer + i) = 0;
	i++;
	*(buffer + i) = 0;
	i++;
	*(buffer + i) = 0x04;
	i++;

	*(buffer + i) = data->r - 128;
	i++;
	*(buffer + i) = data->g - 128;
	i++;
	*(buffer + i) = data->b - 128;
	i++;
	*(buffer + i) = 255-128;
	i++;

	return i;
}

int addSeparator(char * key, char * buffer) {
	int i;
	i = 0;

	uint32_t sep;
	sscanf(key + 2, "%08x", &sep);

	*(buffer) = NBT_SEPARATOR;
	i ++;
	*(buffer + i) = ((uint8_t *)(&sep))[3];
	i++;
	*(buffer + i) = ((uint8_t *)(&sep))[2];
	i++;
	*(buffer + i) = ((uint8_t *)(&sep))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&sep))[0];
	i++;

	return i;
}

int addInt(char * key, int data, char * buffer) {
	int i, length;
	i = 0;
	length = strlen(key);

	*(buffer) = NBT_INT;
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[1];
	i++;
	*(buffer + i) = ((uint8_t *)(&length))[0];
	i++;
	strcpy(buffer + i, key);
	i += strlen(key);

	int j;
	for (j = 0; j < 4; j++) {
		*(buffer + i) = ((uint8_t *)(&data))[3 - j];
		i++;
	}

	return i;
}
