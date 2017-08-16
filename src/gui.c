#include "gui.h"

/**
 * [gui_init description]
 * @param  w          [description]
 * @param  h          [description]
 * @param  r          [description]
 * @param  g          [description]
 * @param  b          [description]
 * @param  disableISO [description]
 * @param  title      [description]
 * @return            [description]
 */
gui *gui_init(int w, int h, int r, int g, int b, int disableISO, char *title) {
  srand(time(NULL));
  gui *out = malloc(sizeof(gui));

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) ==
      -1) {
    fprintf(stderr, "Impossible to init SDL Mixer%s\n", Mix_GetError());
  }
  Mix_AllocateChannels(GUI_MIXER_CHANNEL);
  out->mouvement = NULL;
  out->background_music = NULL;
  out->win = NULL;

  out->screen = NULL;
  if (SDL_Init(SDL_INIT_VIDEO == -1)) {
    fprintf(stderr, "Impossible to init SDL %s\n :", SDL_GetError());
    return NULL;
  }

  out->screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE );
  if (out->screen == NULL) {
    fprintf(stderr, "Impossible to set Video Mode %s\n :", SDL_GetError());
    return NULL;
  }

  out->background = SDL_CreateRGBSurface(SDL_HWSURFACE, (out->screen)->w,
                                         (out->screen)->h, 32, 0, 0, 0, 0);
  SDL_FillRect(out->background, NULL, RGB(out->screen, r, g, b));
  out->wall = NULL;
  out->robotnw = NULL;
  out->robotne = NULL;
  out->robotsw = NULL;
  out->robotse = NULL;
  out->robot = NULL;
  out->sol = NULL;
  out->depart = NULL;
  out->fin = NULL;

  // SDL_EnableKeyRepeat(10, 10);
  out->last_time = SDL_GetTicks();
  out->allow_bot_moves = 0;
  out->fast = 0;
  out->debug = 0;
  out->finished = 0;
  out->togglemove = 0;
  out->lastX = 0;
  out->lastY = 0;
  out->noiso = disableISO;
  if (!disableISO) {
    out->dx = 0;
    out->dy = (out->screen)->h / 4;
  } else {
    out->dx = 0;
    out->dy = 0;
  }

  SDL_WM_SetCaption(title, NULL);

  return out;
}

/**
 * Charge une image et la renvoie
 * @param  path emplacement
 * @return      image chargée
 */
SDL_Surface *gui_loadSprite(char *path) {
  SDL_Surface *out = IMG_Load(path);
  if (out == NULL) {
    fprintf(stderr, "Impossible de charger le fichier %s: %s\n", path,
            SDL_GetError());
  }
  return out;
}

/**
 * Charge une sprite dans un fichier aux coordonnées indiquées
 * @param  path  emplacement
 * @param  x     position axe des abscisses
 * @param  y     position axe des ordonnées
 * @param  wsize largeur
 * @param  hsize hauteur
 * @return       Sprite chargée
 */
SDL_Surface *gui_loadSpriteElement(char *path, int x, int y, int wsize,
                                   int hsize) {
  SDL_Surface *tmp = IMG_Load(path);
  if (tmp == NULL) {
    fprintf(stderr, "Impossible de charger le fichier %s: %s\n", path,
            SDL_GetError());
  }
  SDL_Rect rect_src;
  rect_src.x = x * wsize;
  rect_src.y = y * hsize;
  rect_src.w = wsize;
  rect_src.h = hsize;

  SDL_Surface *out =
      SDL_CreateRGBSurface(SDL_HWSURFACE, wsize, hsize, 32, 0, 0, 0, 0);
  SDL_BlitSurface(tmp, &rect_src, out, NULL);
  SDL_SetColorKey(out, SDL_SRCCOLORKEY, 0);

  return out;
}

/**
 * Charge la musique dans l'interface
 * @param  path chemin de la musique
 * @return      musique chargée
 */
Mix_Chunk *gui_loadSounds(char *path) {
  Mix_Chunk *out = Mix_LoadWAV(path);
  if (out == NULL) {
    printf("Impossible de charger le fichier %s\n", path);
  }

  return out;
}

/**
 * Charge la musique dans l'interface
 * @param  path chemin de la musique
 * @return      musique chargée
 */
Mix_Music *gui_loadMusic(char *path) {
  Mix_Music *out = Mix_LoadMUS(path);
  if (out == NULL) {
    printf("Impossible de charger le fichier%s\n", path);
  }
  return out;
}

/**
 * Joue le son du mouvement
 * @param in interface à utiliser
 */
void gui_playMovement(gui *in) {
  if (in->mouvement != NULL)
    Mix_PlayChannel(-1, in->mouvement, 0);
}

/**
 * Joue la musique de fin
 * @param in interface à utiliser
 */
void gui_playWin(gui *in) {
  if (in->win) {
    Mix_HaltMusic();
    Mix_PlayMusic(in->win, -1);
  }
}

/**
 * Démarre la musique de fond
 * @param in interface à utiliser pour récupérer la musique
 */
void gui_launchBackgroundMusic(gui *in) {
  if (in->background_music != NULL)
    Mix_PlayMusic(in->background_music, -1);
}

/**
 * Gestion des évènements
 * @param in interface à regarder
 */
void gui_eventGesture(gui *in) {
  SDL_Event event;

  in->start_time = SDL_GetTicks();
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_QUIT:
      in->finished = 1;
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case 'q':
        in->finished = 1;
        break;
      case 'p':
        in->allow_bot_moves = 0;
        break;
      case 's':
        in->allow_bot_moves = 1;
        gui_launchBackgroundMusic(in);
        break;
      case 'f':
        in->fast = 1;
        break;
      case 'd':
        if (in->debug) {
          in->debug = 0;
        } else {
          in->debug = 1;
        }
        break;
      case '$':
        if (system("eject")) {
        }
        break;
      default:
        break;
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      if (event.button.button == SDL_BUTTON_LEFT) {
        in->togglemove = 1;
        in->lastX = event.button.x;
        in->lastY = event.button.y;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if (event.button.button == SDL_BUTTON_LEFT) {
        in->togglemove = 0;
        in->lastX = event.button.x;
        in->lastY = event.button.y;
      }
      break;
    case SDL_MOUSEMOTION:
      if (in->togglemove) {
        in->dx += event.button.x - in->lastX;
        in->dy += event.button.y - in->lastY;
        in->lastX = event.button.x;
        in->lastY = event.button.y;
      }
      break;
    default:
      break;
    }
  }
  in->current_time = SDL_GetTicks();
  in->ellapsed_time = in->current_time - in->last_time;
  in->last_time = in->current_time;
}

/**
 * Affiche les composants de l'interface
 * @param in   interface à modifier
 * @param dude robot à bouger
 * @param flat carte à examiner
 */
void gui_draw(gui *in, bot *dude, map *flat) {
  SDL_BlitSurface(in->background, NULL, in->screen, NULL);
  if (!in->noiso) {
    gui_map_draw_iso(in, flat);
    gui_bot_draw_iso(in, dude);
  } else {
    gui_map_draw(in, flat);
    if (in->debug) {
      gui_bot_debugDraw(in, dude);
    } else {
      gui_bot_draw(in, dude);
    }
  }
  SDL_Flip(in->screen);
  if (dude->finished)
    in->allow_bot_moves = 0;
}

/**
 * Attend un délai d'attente - le temps d'execution
 * @param in         interface à bloquer
 * @param prgm_delay délai (ms)
 */
void gui_wait(gui *in, int prgm_delay) {
  in->ellapsed_time = SDL_GetTicks() - in->start_time;
  if ((int)in->ellapsed_time < prgm_delay) {
    SDL_Delay(prgm_delay - in->ellapsed_time);
  }
}

/**
 * Met en pause le programme avant extinction
 */
void gui_pause() {
  SDL_Event event;
  int quit = 0;
  while (!quit) {
    SDL_WaitEvent(&event);
    switch (event.type) {
    case SDL_QUIT:
      quit = 1;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == 'q') {
        quit = 1;
      }
      break;
    default:
      break;
    }
  }
}

/**
 * Libère une interface de la mémoire
 * @param in interface à libérer
 */
void gui_free(gui *in) {
  if (in->background != NULL)
    SDL_FreeSurface(in->background);
  if (in->screen != NULL)
    SDL_FreeSurface(in->screen);
  if (in->sol != NULL)
    SDL_FreeSurface(in->sol);
  if (in->wall != NULL)
    SDL_FreeSurface(in->wall);
  if (in->depart != NULL)
    SDL_FreeSurface(in->depart);
  if (in->robotnw != NULL)
    SDL_FreeSurface(in->robotnw);
  if (in->robotne != NULL)
    SDL_FreeSurface(in->robotne);
  if (in->robotse != NULL)
    SDL_FreeSurface(in->robotse);
  if (in->robotsw != NULL)
    SDL_FreeSurface(in->robotsw);
  if (in->robot != NULL)
    SDL_FreeSurface(in->robot);
  if (in->fin != NULL)
    SDL_FreeSurface(in->fin);
  if (in->mouvement != NULL)
    Mix_FreeChunk(in->mouvement);
  if (in->win != NULL)
    Mix_FreeMusic(in->win);
  if (in->background_music != NULL)
    Mix_FreeMusic(in->background_music);

  Mix_CloseAudio();
  SDL_Quit();
  free(in);
}

/**
 * Affiche un robot dans l'interface
 * @param gui interface dans laquelle afficher
 * @param in  robot à afficher
 */
void gui_bot_draw(gui *gui, bot *in) {
  SDL_Rect rect_dest; // Rectangle destination
  rect_dest.x = in->x * in->wSize;
  rect_dest.y = in->y * in->hSize;
  if (gui->robot != NULL) {
    SDL_Rect rect_src;
    // rect_src.x = (in->dir / NB_SPRITES_P_LINE) * in->wSize;
    rect_src.x = (in->pas % 4) * in->wSize; // Il n'y a que 4 directions
    rect_src.y = (in->dir % NB_SPRITES_P_LINE) * in->hSize;
    rect_src.w = in->wSize;
    rect_src.h = in->hSize;
    SDL_BlitSurface(gui->robot, &rect_src, gui->screen, &rect_dest);
  } else {
    SDL_Surface *value = SDL_CreateRGBSurface(SDL_HWSURFACE, in->wSize,
                                              in->hSize, 32, 0, 0, 0, 0);
    SDL_FillRect(value, NULL, BOT_RED_COLOR);
    SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);
    SDL_FreeSurface(value);
  }
}

/**
 * Affiche un robot dans l'interface (isometrique)
 * @param gui interface dans laquelle afficher
 * @param in  robot à afficher
 */
void gui_bot_draw_iso(gui *gui, bot *in) {
  SDL_Rect rect_dest; // Rectangle destination
  int x = in->x + 3;
  int y = in->y - 3;
  rect_dest.x = (x + y) * in->wSize / 2 + gui->dx;
  rect_dest.y = (y - x) * ISO_CASE_SIZE / 2 + gui->dy;
  if (gui->robotne != NULL && gui->robotnw != NULL && gui->robotse != NULL &&
      gui->robotsw != NULL) {
    SDL_BlitSurface(gui_getRobotSurface(gui, in), NULL, gui->screen,
                    &rect_dest);
  } else {
    SDL_Surface *value = SDL_CreateRGBSurface(SDL_HWSURFACE, in->wSize,
                                              in->hSize, 32, 0, 0, 0, 0);
    SDL_FillRect(value, NULL, BOT_RED_COLOR);
    SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);
    SDL_FreeSurface(value);
  }
}

/**
 * Affiche un robot dans l'interface avec indications mémoire
 * @param gui interface dans laquelle afficher
 * @param in  robot à afficher
 */
void gui_bot_debugDraw(gui *gui, bot *in) {
  SDL_Rect rect_dest; // Rectangle destination
  SDL_Surface *value =
      SDL_CreateRGBSurface(SDL_HWSURFACE, in->wSize, in->hSize, 32, 0, 0, 0, 0);

  bot_memory *tmp = in->nodes;
  for (int i = 0; i < tmp->length; i++) {
    rect_dest.x = tmp->x[i] * in->wSize;
    rect_dest.y = tmp->y[i] * in->hSize;
    if (tmp->left[i] + tmp->right[i] + tmp->up[i] + tmp->down[i] > 0) {
      SDL_FillRect(value, NULL, BOT_NODE_COLOR);
    } else {
      SDL_FillRect(value, NULL, BOT_USEDNODE_COLOR);
    }
    SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);
  }

  rect_dest.x = in->x * in->wSize;
  rect_dest.y = in->y * in->hSize;
  SDL_FillRect(value, NULL, BOT_RED_COLOR);
  SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);

  SDL_FreeSurface(value);
}

/**
 * Obtient le sprite adéquat en fonction de la direction
 * @param  gui interface à prendre
 * @param  in  robot à examiner
 * @return     surface de l'interface retenue
 */
SDL_Surface *gui_getRobotSurface(gui *gui, bot *in) {
  switch (in->dir) {
  case LEFT:
    return gui->robotsw;
    break;
  case RIGHT:
    return gui->robotne;
    break;
  case UP:
    return gui->robotnw;
    break;
  case DOWN:
    return gui->robotse;
    break;
  default:
    return NULL;
    break;
  }
}

/**
 * Affiche la map dans un interface
 * @param gui interface à utiliser
 * @param in Carte à afficher
 */
void gui_map_draw(gui *gui, map *in) {
  SDL_Surface *value =
      SDL_CreateRGBSurface(SDL_HWSURFACE, in->size, in->size, 32, 0, 0, 0, 0);
  SDL_Rect rect_dest;

  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      rect_dest.x = j * in->size;
      rect_dest.y = i * in->size;
      switch (in->data[i][j]) {
      case 'D':
        SDL_FillRect(value, NULL, MAP_D_CASE_COLOR);
        break;
      case ' ':
        SDL_FillRect(value, NULL, MAP_FREE_CASE_COLOR);
        break;
      case 'x':
        SDL_FillRect(value, NULL, MAP_X_CASE_COLOR);
        break;
      case 'S':
        SDL_FillRect(value, NULL, MAP_S_CASE_COLOR);
        break;
      default:
        SDL_FillRect(value, NULL, MAP_UNKNOWN_CASE_COLOR);
        break;
      }

      if (in->data[i][j] == 'x' && gui->wall != NULL) {
        SDL_BlitSurface(gui->wall, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == ' ' && gui->sol != NULL) {
        SDL_BlitSurface(gui->sol, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == 'D' && gui->depart != NULL) {
        SDL_BlitSurface(gui->depart, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == 'S' && gui->fin != NULL) {
        SDL_BlitSurface(gui->fin, NULL, gui->screen, &rect_dest);
      } else {
        SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);
      }
    }
  }

  SDL_FreeSurface(value);
}

/**
 * Affiche la map dans un interface (isometrique)
 * @param gui interface à utiliser
 * @param in Carte à afficher
 */
void gui_map_draw_iso(gui *gui, map *in) {
  SDL_Surface *value =
      SDL_CreateRGBSurface(SDL_HWSURFACE, in->size, in->size, 32, 0, 0, 0, 0);
  SDL_Rect rect_dest;

  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      int x = (j + i) * (gui->sol)->w / 2 + gui->dx;
      int y = (i - j) * (gui->sol)->h / 2 + gui->dy;
      rect_dest.x = x;
      rect_dest.y = y;
      switch (in->data[i][j]) {
      case 'D':
        SDL_FillRect(value, NULL, MAP_D_CASE_COLOR);
        break;
      case ' ':
        SDL_FillRect(value, NULL, MAP_FREE_CASE_COLOR);
        break;
      case 'x':
        SDL_FillRect(value, NULL, MAP_X_CASE_COLOR);
        break;
      case 'S':
        SDL_FillRect(value, NULL, MAP_S_CASE_COLOR);
        break;
      default:
        SDL_FillRect(value, NULL, MAP_UNKNOWN_CASE_COLOR);
        break;
      }

      if (in->data[i][j] == 'x' && gui->wall != NULL) {
        SDL_BlitSurface(gui->wall, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == ' ' && gui->sol != NULL) {
        SDL_BlitSurface(gui->sol, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == 'D' && gui->depart != NULL) {
        SDL_BlitSurface(gui->depart, NULL, gui->screen, &rect_dest);
      } else if (in->data[i][j] == 'S' && gui->fin != NULL) {
        SDL_BlitSurface(gui->fin, NULL, gui->screen, &rect_dest);
      } else {
        SDL_BlitSurface(value, NULL, gui->screen, &rect_dest);
      }
    }
  }

  SDL_FreeSurface(value);
}
