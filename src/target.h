#ifndef TARGET_H
#define TARGET_H

#include <stdio.h>

#define STR 	1
#define COL 	2
#define VAL 	3
#define BOOL	4
#define GRUP	5
#define SEP		6
#define TARGET_INT		7

#define TARGETCOUNT 735
#define TARGETCOUNTSIMPLE 70
#define PUSHTARGET(R,X,T)	sprintf((R)[target_at], "%c%s", (T), (X)); target_at++

void importTargets(char ** target);
void importTargetsSimple(char ** target);

#endif
