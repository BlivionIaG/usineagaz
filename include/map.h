#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"

#define MAX_LAB_WIDTH 8192

#define MAP_WALL_CASE 'x'
#define MAP_FREE_CASE ' '
#define MAP_START_CASE 'D'
#define MAP_STOP_CASE 'S'

#define MAP_D_CASE_COLOR 0xFFFF00
#define MAP_FREE_CASE_COLOR 0xFFFFFF
#define MAP_X_CASE_COLOR 0x000000
#define MAP_S_CASE_COLOR 0x00FF00
#define MAP_UNKNOWN_CASE_COLOR 0x593001

typedef struct map {
  char **data;
  int w, h, size;
} map;

map *map_load(char *path, int size);
void map_print(map *in);

extra_coords map_getEntry(map *in);
extra_coords map_getExit(map *in);

int map_equals(map *in, int x, int y, char c);

void map_free(map *in);

#endif /* MAP_H */