#ifndef GUI_H
#define GUI_H

/* SDL libs */
#include <SDL/SDL.h>        // -lSDL
#include <SDL/SDL_image.h>  // -lSDL_image
#include <SDL/SDL_ttf.h>    // -lSDL_ttf
#include <time.h>
#include "bot.h"
#include "map.h"

#define RGB(ecran, a, b, c) SDL_MapRGB(ecran->format, a, b, c)

typedef struct gui {
  SDL_Surface *screen, *background;
  Uint32 last_time, current_time, ellapsed_time, start_time;
  int allow_bot_moves, fast, debug, finished;
} gui;

gui *gui_init(int w, int h, int r, int g, int b);
void gui_eventGesture(gui *in);
void gui_draw(gui *in, bot *dude, map *flat);
void gui_wait(gui *in, int prgm_delay);
void gui_pause();

void gui_free(gui *in);

#endif /* GUI_H */
