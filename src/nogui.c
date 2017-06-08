#include "nogui.h"

void nogui_bot_on_map_draw(bot *in, map *map) {
  int tmp = system("clear");
  for (int i = 0; i < map->h; i++) {
    for (int j = 0; j < map->w; j++) {
      if (in->x == j && in->y == i) {
        printf("%c", NOGUI_BOT);
        fflush(stdout);
      } else {
        printf("%c", map->data[i][j]);
        fflush(stdout);
      }
    }
    printf("\n");
    fflush(stdout);
  }
}