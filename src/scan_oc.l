
A	[a-zA-Z]
D	[0-9]
S	[:{}]
K	({A}|{D}|"_")

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR	1
#define COL	2
#define VAL	3
#define BOOL	4

#define TYPES	"STR\0COL\0VAL\0BOOL"

int row = 1;
int dpth = 0;
int type = 0;

char * key = NULL;

void get_key();
void new_line();
%}

%%

"\""{K}+"\":"	{ get_key(); }
"\""{A}*"\""	{ type = STR; }
("true"|"false")	{ type = BOOL; }
{D}*(".")?{D}*	{ type = VAL; }
"{"	{ dpth++; type = COL; }
"}"	{ dpth--; /*if (!depth) new_line();*/ }

"\n"	{ row++; if (dpth && key) new_line(); }
"},"	{ type = 0; key[0] = 0; dpth--; }
.	{ /* */ }

%%

void get_key() {
	if (!key) {
		key = malloc(sizeof(*key)*256);
		//key[0] = 0;
	}
	if (dpth) {
		strcpy(key, yytext);
		key[strlen(key)-1] = 0;
	}
}

void new_line() {
	if (type && ((dpth & 1) || (type == COL && dpth == 2))) 
		printf("PUSHTARGET(target, %s, %s);\n", key, TYPES + (type - 1)*4);
	else if (!type && (dpth & 1) && key[0]) {
		//printf("\terror: %s with no type\n", key);
	}
	type = 0;
}
