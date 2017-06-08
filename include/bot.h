#ifndef BOT_H
#define BOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"
#include "map.h"

#define BOT_RED_COLOR 0xFF0000
#define BOT_NODE_COLOR 0xFF00FF
#define BOT_USEDNODE_COLOR 0x440044

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3

typedef struct bot_memory {
  int *x, *y;
  int *left, *right, *up, *down;
  int length;
} bot_memory;

typedef struct bot {
  int x, y, wSize, hSize, dir;  // Tailles et positions
  int left, right, up, down;    // Directions courantes
  bot_memory *nodes;            // Mémoire noeuds
  extra_coords *history;        // Historique positions
  int historyLength;            // Taille de l'historique
  int pas, debug, finished;     // Pas et fini ( bool )
} bot;

bot *bot_init(int x, int y, int wSize, int hSize, int mute);

bot_memory *bot_memory_init();
void bot_pushNode(bot *in);

int bot_memory_position(bot_memory *nodes, int x, int y);
int bot_memory_nodesCheck(bot *in, int x, int y);

void bot_pushHistory(bot *in);
void bot_popHistory(bot *in);
int bot_historyCheck(bot *in, int x, int y);

void bot_move(map *map, bot *in);

void bot_move_left(bot *in);
void bot_move_right(bot *in);
void bot_move_up(bot *in);
void bot_move_down(bot *in);
void bot_move_back(bot *in);
void bot_checkExit(bot *in, map *map);

void bot_rotate(bot *in, int new_dir);
void bot_reverseDir(bot *in);

void bot_free(bot *in);
void bot_memory_free(bot_memory *in);

#endif /* BOT_H */