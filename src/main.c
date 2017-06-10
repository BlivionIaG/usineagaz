#include "bot.h"
#include "gui.h"
#include "map.h"
#include "nogui.h"

#define CASE_SIZE 16
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PRGM_DELAY 100

int main(int argc, char **argv) {
  int terminal_only = 0, gui_only = 0;
  if (argc < 2) {
    printf("USAGE : ./usine_a_gaz.bin map\n\tOR  ./usine_a_gaz.bin map "
           "guioption\n "
           "\t\t\t\tg = gui, t = nogui\n");
    return -1;
  }

  if (argc == 3) {
    if (!strcmp(argv[2], "g")) {
      gui_only = 1;
    } else if (!strcmp(argv[2], "t")) {
      terminal_only = 1;
    } else {
      printf("Not a valid option, g = gui_only, t = terminal_only\n");
    }
  }

  /* Chargement de la carte */
  map *terrain = map_load(argv[1], CASE_SIZE);
  if (terrain == NULL) {
    fprintf(stderr, "Impossible to loadMap%s\n :", SDL_GetError());
    return -2;
  }

  /* Initialisation du robot */
  bot *jean_claude = bot_init(map_getEntry(terrain).x, map_getEntry(terrain).y,
                              CASE_SIZE, CASE_SIZE, 0);
  if (jean_claude == NULL) {
    fprintf(stderr, "Impossible to load Jean Claude :(%s\n :", SDL_GetError());
    return -3;
  }

  /* Initialisation de l'interface graphique */
  if (!terminal_only) { // Mode interface graphique
    gui *interface =
        gui_init(terrain->w * CASE_SIZE, terrain->h * CASE_SIZE, 192, 192, 192);
    interface->wall = gui_loadSprite("ressources/wall.png");
    interface->robot = gui_loadSprite("ressources/player.png");
    interface->depart = gui_loadSprite("ressources/start.png");
    interface->fin = gui_loadSprite("ressources/end.png");
    interface->sol = gui_loadSprite("ressources/floor.png");
    interface->mouvement = gui_loadSounds("ressources/mouvement.wav");
    interface->background_music = gui_loadMusic("ressources/bgmusic.mod");
    interface->win = gui_loadMusic("ressources/win.mp3");

    int *allow_bot_moves = &(interface->allow_bot_moves);

    while (!interface->finished) { // Tant que l'interface n'a pas terminée
      gui_eventGesture(interface); // Gestion des évènements
      gui_draw(interface, jean_claude,
               terrain); // Affichage de l'interface et des éléments

      if (*allow_bot_moves && !gui_only) // Si on peut faire bouger les robots
                                         // et on a le droit d'afficher dans le
                                         // terminal
        nogui_bot_on_map_draw(jean_claude, terrain);

      gui_wait(interface, PRGM_DELAY); // On attend

      if (interface->fast) {
        while (!jean_claude->finished) {
          bot_move(terrain, jean_claude);
        }
        bot_move(terrain, jean_claude);
      } else {
        if (interface->allow_bot_moves) {
          bot_move(terrain, jean_claude);
          // gui_playMovement(interface, jean_claude);
        }
      }
    }
    interface->allow_bot_moves = 0;

    gui_draw(interface, jean_claude, terrain);
    gui_pause();
    gui_free(interface);

  } else {
    while (!jean_claude->finished) {
      bot_move_safe(terrain, jean_claude);
      nogui_bot_on_map_draw(jean_claude, terrain);
      usleep(PRGM_DELAY * 1000);
    }
  }

  if (!gui_only) {
    nogui_bot_on_map_draw(jean_claude, terrain);
    printf("PAS :%d\n", jean_claude->pas);
  }

  map_free(terrain);
  bot_free(jean_claude);

  return 0;
}
