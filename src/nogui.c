#include "nogui.h"

/**
 * Affiche la Carte et le Robot dans le terminal
 * @param in  Robot que l'on veut afficher dans le terminal
 * @param map Carte que l'on veut afficher dans le terminal
 */
void nogui_bot_on_map_draw(bot *in, map *map) {
  int tmp = system("clear");
  for (int i = 0; i < map->h; i++) {
    for (int j = 0; j < map->w; j++) {
      if (in->x == j && in->y == i) {
        printf("%c", NOGUI_BOT);
      } else {
        printf("%c", map->data[i][j]);
      }
    }
    printf("\n");
    fflush(stdout);
  }
}
