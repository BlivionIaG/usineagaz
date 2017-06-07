#ifndef BOT_H
#define BOT_H

#include <SDL/SDL.h>  // -lSDL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"
#include "map.h"

#define BOT_RED_COLOR 0xFF0000

typedef struct bot_memory {
  int *x, *y;
  int *left, *right, *up, *down;
  int length;
} bot_memory;

typedef struct bot {
  int x, y, wSize, hSize;     // Tailles et positions
  int left, right, up, down;  // Directions courantes
  bot_memory *nodes;          // Mémoire noeuds
  extra_coords *history;      // Historique positions
  int historyLength;          // Taille de l'historique
  int pas, finished;          // Pas et fini ( bool )
} bot;

bot *bot_init(int x, int y, int wSize, int hSize);

bot_memory *bot_memory_init();
void bot_pushNode(bot *in);

int bot_memory_position(bot_memory *nodes, int x, int y);

void bot_pushHistory(bot *in);
void bot_popHistory(bot *in);
int bot_historyCheck(bot *in, int x, int y);

void bot_draw(SDL_Surface *screen, bot *in);
void bot_move(map *map, bot *in);

void bot_free(bot *in);
void bot_memory_free(bot_memory *in);

#endif /* BOT_H */