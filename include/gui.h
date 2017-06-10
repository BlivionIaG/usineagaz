#ifndef GUI_H
#define GUI_H

/* SDL libs */
#include "bot.h"
#include "map.h"
#include <SDL/SDL.h>       // -lSDL
#include <SDL/SDL_image.h> // -lSDL_image
#include <SDL/SDL_mixer.h> // -lSDL_mixer
#include <SDL/SDL_ttf.h>   // -lSDL_ttf
#include <time.h>

#define RGB(ecran, a, b, c) SDL_MapRGB(ecran->format, a, b, c)
#define NB_SPRITES_P_LINE 4
#define GUI_MIXER_CHANNEL 10

typedef struct gui {
  SDL_Surface *screen, *background;
  SDL_Surface *wall, *sol, *robot, *depart, *fin;
  Mix_Chunk *mouvement;
  Mix_Music *background_music, *win;
  Uint32 last_time, current_time, ellapsed_time, start_time;
  int allow_bot_moves, fast, debug, finished;
} gui;

gui *gui_init(int w, int h, int r, int g, int b);
SDL_Surface *gui_loadSprite(char *path);
Mix_Chunk *gui_loadSounds(char *path);
Mix_Music *gui_loadMusic(char *path);
void gui_playMovement(gui *in, bot *bot);
void gui_launchBackgroundMusic(gui *in);

void gui_eventGesture(gui *in);
void gui_draw(gui *in, bot *dude, map *flat);
void gui_wait(gui *in, int prgm_delay);
void gui_pause();

void gui_bot_draw(gui *gui, bot *in);
void gui_bot_debugDraw(gui *gui, bot *in);

void gui_map_draw(gui *gui, map *mymap);

void gui_free(gui *in);

#endif /* GUI_H */
