#ifndef EXTRA_H
#define EXTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct extra_coords { int x, y; } extra_coords;

char **extra_split(int *outlen, const char *c_in, const char *c_param);
int extra_fgets(char *s, int size, FILE *stream);
extra_coords extra_new_coords(int x, int y);
int extra_randomInt(int nMin, int nMax);

#endif /* EXTRA_H */