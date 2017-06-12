#include "bot.h"
#include "gui.h"
#include "map.h"
#include "nogui.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PRGM_DELAY 100

int main(int argc, char **argv) {
  int speed = PRGM_DELAY;
  int terminal_only = 0, gui_only = 0, non_isometric = 0, force_safe = 0;
  if (argc < 2) {
    printf("USAGE : ./usine_a_gaz.bin map\n\tOR  ./usine_a_gaz.bin map "
           "guioption\n "
           "\t\t\t\tg = gui, t = nogui\n f = force safe move\nn = disable "
           "isometric 2D\n");
    return -1;
  }

  if (argc >= 3) {
    if (!strcmp(argv[2], "g")) { // mode graphique uniquement
      gui_only = 1;
    } else if (!strcmp(argv[2], "t")) { // terminal uniquement
      terminal_only = 1;
    } else if (!strcmp(argv[2], "n")) { // 2D normale
      non_isometric = 1;
    } else if (!strcmp(argv[2], "fn") ||
               !strcmp(argv[2], "nf")) { // Algo 2 et 2D
      non_isometric = 1;
      force_safe = 1;
    } else if (!strcmp(argv[2], "f")) { // algo 2
      force_safe = 1;
    } else {
      printf("Not a valid option, g = gui_only, t = terminal_only\n");
    }
  }

  if (argc == 4) { // Quand délai précisé
    speed = atoi(argv[3]);
  }

  /* Chargement de la carte */
  map *terrain = map_load(argv[1], CASE_SIZE);
  if (terrain == NULL) {
    fprintf(stderr, "Impossible to loadMap%s\n :", SDL_GetError());
    return -2;
  }

  /* Initialisation du robot */
  bot *jean_claude = NULL;
  if (non_isometric) { // 2D
    jean_claude = bot_init(map_getEntry(terrain).x, map_getEntry(terrain).y,
                           CASE_SIZE, CASE_SIZE, 0);
  } else { // ISO
    jean_claude = bot_init(map_getEntry(terrain).x, map_getEntry(terrain).y,
                           ROBW, ROBH, 0);
  }
  if (jean_claude == NULL) {
    fprintf(stderr, "Impossible to load Jean Claude :(%s\n :", SDL_GetError());
    return -3;
  }

  /* Initialisation de l'interface graphique */
  if (!terminal_only) { // Mode interface graphique
    gui *interface = NULL;
    if (!non_isometric) { // Chargement des ressources (iso)
      interface = gui_init(SCREEN_WIDTH, SCREEN_HEIGHT, 192, 192, 192,
                           non_isometric, "Projet Informatique C");
      interface->wall =
          gui_loadSpriteElement("ressources/grassland_tiles.png", 0, 19,
                                2 * ISO_CASE_SIZE, ISO_CASE_SIZE);
      interface->robotnw = gui_loadSprite("ressources/man-nw.png");
      interface->robotne = gui_loadSprite("ressources/man-ne.png");
      interface->robotsw = gui_loadSprite("ressources/man-sw.png");
      interface->robotse = gui_loadSprite("ressources/man-se.png");
      interface->depart =
          gui_loadSpriteElement("ressources/grassland_tiles.png", 0, 1,
                                2 * ISO_CASE_SIZE, ISO_CASE_SIZE);
      interface->fin =
          gui_loadSpriteElement("ressources/grassland_tiles.png", 0, 1,
                                2 * ISO_CASE_SIZE, ISO_CASE_SIZE);
      interface->sol =
          gui_loadSpriteElement("ressources/grassland_tiles.png", 0, 0,
                                2 * ISO_CASE_SIZE, ISO_CASE_SIZE);
    } else { // Chargement des ressources (2D)
      interface = gui_init(terrain->w * CASE_SIZE, terrain->h * CASE_SIZE, 192,
                           192, 192, non_isometric, "Projet Informatique C");
      interface->wall = gui_loadSprite("ressources/wall.png");
      interface->robot = gui_loadSprite("ressources/player.png");
      interface->depart = gui_loadSprite("ressources/start.png");
      interface->fin = gui_loadSprite("ressources/end.png");
      interface->sol = gui_loadSprite("ressources/floor.png");
    }
    // Sons
    interface->mouvement = gui_loadSounds("ressources/mouvement.wav");
    interface->background_music = gui_loadMusic("ressources/bgmusic.wav");
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

      gui_wait(interface, speed); // On attend

      if (interface->fast) { // Mode instentané
        while (!jean_claude->finished) {
          if (force_safe) {
            bot_move_safe(terrain, jean_claude);
          } else {
            bot_move(terrain, jean_claude);
          }
        }
        if (force_safe) {
          bot_move_safe(terrain, jean_claude);
        } else {
          bot_move(terrain, jean_claude);
        }
      } else { // sinon
        if (interface->allow_bot_moves) {
          if (force_safe) {
            bot_move_safe(terrain, jean_claude);
          } else {
            bot_move(terrain, jean_claude);
          }
          // gui_playMovement(interface);
        }
      }
      // interface->finished = jean_claude->finished;
    }
    gui_playWin(interface); // Son de victoire
    interface->allow_bot_moves = 0;

    printf("PAS :%d\n", jean_claude->pas);
    gui_draw(interface, jean_claude, terrain);
    gui_pause();
    gui_free(interface);

  } else { // Terminal uniquement
    while (!jean_claude->finished) {
      bot_move_safe(terrain, jean_claude);
      nogui_bot_on_map_draw(jean_claude, terrain);
      usleep(speed * 1000);
    }
    printf("PAS :%d\n", jean_claude->pas);
  }

  if (!gui_only) {
    nogui_bot_on_map_draw(jean_claude, terrain);
  }

  printf("PAS :%d\n", jean_claude->pas);
  map_free(terrain);
  bot_free(jean_claude);

  return 0;
}
