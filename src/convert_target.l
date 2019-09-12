%{

void get_target (char * s);

%}
%%

"PUSHTARGET(target, \""[a-zA-Z0-9_]*"\", VAL);"	{ get_target("VAL");}
"PUSHTARGET(target, \""[a-zA-Z0-9_]*"\", COL);" { get_target("COL"); }
"PUSHTARGET(target, \""[a-zA-Z0-9_]*"\", BOOL);" { get_target("BOOL"); }
"PUSHTARGET(target, \""[a-zA-Z0-9_]*"\", STR);" { get_target("STR"); }
"\n"	{ }
.	{ }

%%

void get_target(char * s) {
	char buf[256];
	strncpy(buf, yytext + 20, 256);
	*strchr(buf, '\"') = 0;

	puts("\t{");
	printf("\t\tname=\"%s\",\n", buf);
	printf("\t\tttype=%s\n", s);
	puts("\t},");
}

