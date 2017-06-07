#include "bot.h"
#include "gui.h"
#include "map.h"

#define CASE_SIZE 8
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PRGM_DELAY 10

int main(int argc, char **argv) {
  if (argc != 2) return -1;  // Si nombre arguments invalides

  map *terrain = map_load(argv[1], CASE_SIZE);
  map_print(terrain);
  if (terrain == NULL) {
    fprintf(stderr, "Impossible to loadMap%s\n :", SDL_GetError());
    exit(-2);
  }

  bot *jean_claude = bot_init(map_getEntry(terrain).x, map_getEntry(terrain).y,
                              CASE_SIZE, CASE_SIZE);
  if (jean_claude == NULL) {
    fprintf(stderr, "Impossible to load Jean Claude :(%s\n :", SDL_GetError());
    exit(-3);
  }

  gui *interface =
      gui_init(terrain->w * CASE_SIZE, terrain->h * CASE_SIZE, 192, 192, 192);

  while (!jean_claude->finished) {
    gui_eventGesture(interface, &(jean_claude->finished));
    gui_draw(interface, jean_claude, terrain);
    gui_wait(interface, PRGM_DELAY);

    if (interface->fast) {
      while (!jean_claude->finished) {
        bot_move(terrain, jean_claude);
      }
    }
  }

  interface->allow_bot_moves = 0;
  gui_draw(interface, jean_claude, terrain);
  gui_pause();

  gui_free(interface);
  map_free(terrain);
  bot_free(jean_claude);

  return 0;
}
