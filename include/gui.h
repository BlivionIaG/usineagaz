#ifndef GUI_H
#define GUI_H

/* SDL libs */
#include "bot.h"
#include "map.h"
#include <SDL/SDL.h>       // -lSDL
#include <SDL/SDL_image.h> // -lSDL_image
#include <SDL/SDL_mixer.h> // -lSDL_mixer
#include <SDL/SDL_ttf.h>   // -lSDL_ttf
#include <math.h>
#include <time.h>

#define NB_SPRITES_P_LINE 4 // Sprites par ligne pour
#define RGB(ecran, a, b, c)                                                    \
  SDL_MapRGB(ecran->format, a, b, c) // Conversion rgb vers Uint32
#define GUI_MIXER_CHANNEL 64         // Nombre cannaux son
#define CASE_SIZE 16                 // Taille coté case 2D
#define ISO_CASE_SIZE 32             // Taille coté case 2.5D
#define ROBW 64                      // Largeur robot
#define ROBH 128                     // hauteur robot

typedef struct gui {                      // Structure interface utilisateur
  SDL_Surface *screen, *background;       // Ecran et fond d'écran
  SDL_Surface *wall, *sol, *depart, *fin; // Sprite mur, sol, départ , arrivé
  SDL_Surface *robotnw, *robotne, *robotse, *robotsw,
      *robot;           // Sprite robot et directions ( iso )
  Mix_Chunk *mouvement; // Son mouvement, non utilisé car désagréable
  Mix_Music *background_music, *win; // Musique de fond et de fin
  Uint32 last_time, current_time, ellapsed_time, start_time; // Décalage du
                                                             // temps
  int allow_bot_moves, fast, debug, finished, togglemove; // options booléennes
  int lastX, lastY, dx, dy,
      noiso; // Décalage pour le déplacement en mode isométrique
} gui;

gui *gui_init(int w, int h, int r, int g, int b, int disableISO, char *title);
SDL_Surface *gui_loadSprite(char *path);
SDL_Surface *gui_loadSpriteElement(char *path, int x, int y, int wsize,
                                   int hsize);
Mix_Chunk *gui_loadSounds(char *path);
Mix_Music *gui_loadMusic(char *path);
void gui_playMovement(gui *in);
void gui_playWin(gui *in);
void gui_launchBackgroundMusic(gui *in);
SDL_Surface *gui_getRobotSurface(gui *gui, bot *in);

void gui_eventGesture(gui *in);
void gui_draw(gui *in, bot *dude, map *flat);
void gui_wait(gui *in, int prgm_delay);
void gui_pause();

void gui_bot_draw(gui *gui, bot *in);
void gui_bot_draw_iso(gui *gui, bot *in);
void gui_bot_debugDraw(gui *gui, bot *in);

void gui_map_draw(gui *gui, map *mymap);
void gui_map_draw_iso(gui *gui, map *mymap);

void gui_free(gui *in); // Libère l'interface de la mémoire

#endif /* GUI_H */
