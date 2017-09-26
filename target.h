#ifndef TARGET_H
#define TARGET_H

#include <stdio.h>

#define STR 	1
#define COL 	2
#define VAL 	3
#define BOOL	4

#define TARGETCOUNT 778
#define PUSHTARGET(R,X,T)	sprintf((R)[target_at], "%c%s", (T), (X)); target_at++
int target_at;

void importTargets(char ** target);

#endif