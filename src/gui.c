#include "gui.h"

gui *gui_init(int w, int h, int r, int g, int b) {
  srand(time(NULL));
  gui *out = malloc(sizeof(gui));

  out->screen = NULL;
  if (SDL_Init(SDL_INIT_VIDEO == -1)) {
    fprintf(stderr, "Impossible to init SDL %s\n :", SDL_GetError());
    return NULL;
  }

  out->screen = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if (out->screen == NULL) {
    fprintf(stderr, "Impossible to set Video Mode %s\n :", SDL_GetError());
    return NULL;
  }

  out->background = SDL_CreateRGBSurface(SDL_HWSURFACE, (out->screen)->w,
                                         (out->screen)->h, 32, 0, 0, 0, 0);
  SDL_FillRect(out->background, NULL, RGB(out->screen, r, g, b));
  out->wall = NULL;
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

  return out;
}

SDL_Surface *gui_loadSprite(char *path) {
  SDL_Surface *out = IMG_Load(path);
  if (out == NULL) {
    fprintf(stderr, "Impossible de charger le fichier %s: %s\n", path,
            SDL_GetError());
  }
  return out;
}

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
      default:
        break;
    }
  }
  in->current_time = SDL_GetTicks();
  in->ellapsed_time = in->current_time - in->last_time;
  in->last_time = in->current_time;
}

void gui_draw(gui *in, bot *dude, map *flat) {
  SDL_BlitSurface(in->background, NULL, in->screen, NULL);
  gui_map_draw(in, flat);
  if (in->debug) {
    gui_bot_debugDraw(in, dude);
  } else {
    gui_bot_draw(in, dude);
  }
  SDL_Flip(in->screen);
  if (dude->finished) in->allow_bot_moves = 0;
}

void gui_wait(gui *in, int prgm_delay) {
  in->ellapsed_time = SDL_GetTicks() - in->start_time;
  if ((int)in->ellapsed_time < prgm_delay) {
    SDL_Delay(prgm_delay - in->ellapsed_time);
  }
}

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

void gui_free(gui *in) {
  SDL_FreeSurface(in->background);
  SDL_FreeSurface(in->screen);
  SDL_Quit();
  free(in);
}

void gui_bot_draw(gui *gui, bot *in) {
  SDL_Rect rect_dest;  // Rectangle destination
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

void gui_bot_debugDraw(gui *gui, bot *in) {
  SDL_Rect rect_dest;  // Rectangle destination
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