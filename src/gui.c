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

  SDL_EnableKeyRepeat(10, 10);
  out->last_time = SDL_GetTicks();
  out->allow_bot_moves = 0;
  out->fast = 0;

  return out;
}

void gui_eventGesture(gui *in, int *finished) {
  SDL_Event event;

  in->start_time = SDL_GetTicks();
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        *finished = 1;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case 'q':
            *finished = 1;
            break;
          case 'p':
            in->allow_bot_moves = 0;
            break;
          case 's':
            in->allow_bot_moves = 1;
            break;
          case 'k':
            in->fast = 1;
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
  map_draw(in->screen, flat);
  bot_draw(in->screen, dude);
  SDL_Flip(in->screen);
  if (in->allow_bot_moves) {
    bot_move(flat, dude);
  }
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