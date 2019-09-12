#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "pngimg.h"
#include "nbt.h"

void read_nbt_string(char * s, int fd);

typedef union {
	float f;
	u_int8_t a[4];
} float_bin;

int main(int argc, char * argv[]) {
	if (argc != 2) {
		printf("expected 2 arguments, found %d\n", argc);
		printf("usage: %s <filename.dat>\n", argv[0]);
		return 0;
	}

	int fd;
	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("error: %s isn't accessible\n", argv[1]);
		return 0;
	}

	nbt_t nbt; // main NBT
	int nbt_len, nbt_at;
	nbt_len = 64;
	nbt_at = 0;

	nbt.code = NBT_GROUP;
	nbt.name_length = 4;
	strncpy(nbt.name, "data", nbt.name_length);
	nbt.payload = malloc(sizeof(nbt_t) * nbt_len);

	u_int8_t c;
	char s[256];
	u_int32_t l;
	float_bin f;
	while (read(fd, &c, 1)) {
		l = 0;
		s[0] = 0;
		switch (c) {
			case NBT_GROUP:
				read_nbt_string(s, fd);
			break;
			case NBT_FLOAT:
				read_nbt_string(s, fd);
				read(fd, &f.a[3], 1);
				read(fd, &f.a[2], 1);
				read(fd, &f.a[1], 1);
				read(fd, &f.a[0], 1);
			break;
			case NBT_STRING:
				read_nbt_string(s, fd);
				read_nbt_string(s, fd);
			break;
			case NBT_BOOLEAN:
				read_nbt_string(s, fd);
				read(fd, &c, 1);
			break;
			case NBT_COLOR:
				read_nbt_string(s, fd);
				read(fd, &l, 4);
				read(fd, &f, 4);
			break;
		}
	}


	close(fd);
}

void read_nbt_string(char * s, int fd) {
	u_int8_t c;
	u_int32_t l;
	l = 0;
	read(fd, &c, 1);
	read(fd, &l, 1);
	l |= c << 8;
	if (l > 256)
		l = 256;
	read(fd, s, l);
	s[l] = 0;
}


