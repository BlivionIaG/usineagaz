#include "bot.h"

bot *bot_init(int x, int y, int wSize, int hSize) {
  bot *out = malloc(sizeof(bot));
  out->x = x;
  out->y = y;
  out->wSize = wSize;
  out->hSize = hSize;
  out->history = NULL;
  out->historyLength = 0;
  out->nodes = bot_memory_init();
  out->left = 0;
  out->right = 0;
  out->up = 0;
  out->down = 0;
  out->finished = 0;
  out->pas = 0;

  return out;
}

bot_memory *bot_memory_init() {
  bot_memory *out = malloc(sizeof(bot_memory));
  out->x = NULL;
  out->y = NULL;
  out->left = NULL;
  out->right = NULL;
  out->up = NULL;
  out->down = NULL;
  out->length = 0;

  return out;
}

void bot_pushNode(bot *in) {
  bot_memory *tmp = in->nodes;
  int pos = bot_memory_position(tmp, in->x, in->y);

  if (pos < 0) {
    tmp->length++;
    tmp->x = realloc(tmp->x, tmp->length * sizeof(int));
    tmp->y = realloc(tmp->y, tmp->length * sizeof(int));
    tmp->left = realloc(tmp->left, tmp->length * sizeof(int));
    tmp->right = realloc(tmp->right, tmp->length * sizeof(int));
    tmp->up = realloc(tmp->up, tmp->length * sizeof(int));
    tmp->down = realloc(tmp->down, tmp->length * sizeof(int));

    tmp->x[tmp->length - 1] = in->x;
    tmp->y[tmp->length - 1] = in->y;
    tmp->left[tmp->length - 1] = in->left;
    tmp->right[tmp->length - 1] = in->right;
    tmp->up[tmp->length - 1] = in->up;
    tmp->down[tmp->length - 1] = in->down;
  } else {
    tmp->left[pos] = in->left;
    tmp->right[pos] = in->right;
    tmp->up[pos] = in->up;
    tmp->down[pos] = in->down;
  }
}

int bot_memory_position(bot_memory *nodes, int x, int y) {
  if (nodes->length > 0) {
    for (int i = 0; i < nodes->length; i++) {
      if (x == nodes->x[i] && y == nodes->y[i]) {
        return i;
      }
    }
  }

  return -1;
}

void bot_pushHistory(bot *in) {
  in->historyLength++;
  in->history =
      realloc(in->history, (in->historyLength) * sizeof(extra_coords));

  extra_coords *tmp = in->history;
  tmp[in->historyLength - 1].x = in->x;
  tmp[in->historyLength - 1].y = in->y;
}

void bot_popHistory(bot *in) {
  if (in->historyLength > 0) {
    extra_coords *tmp = in->history;
    in->x = tmp[in->historyLength - 1].x;
    in->y = tmp[in->historyLength - 1].y;

    in->historyLength--;
    in->history =
        realloc(in->history, (in->historyLength) * sizeof(extra_coords));
  }
}

int bot_historyCheck(bot *in, int x, int y) {
  extra_coords *tmp = in->history;
  for (int i = 0; i < in->historyLength; i++) {
    if (tmp[i].x == x && tmp[i].y == y) return 1;
  }

  return 0;
}

void bot_draw(SDL_Surface *screen, bot *in) {
  SDL_Rect rect_dest;  // Rectangle destination
  rect_dest.x = in->x * in->wSize;
  rect_dest.y = in->y * in->hSize;
  SDL_Surface *value =
      SDL_CreateRGBSurface(SDL_HWSURFACE, in->wSize, in->hSize, 32, 0, 0, 0, 0);
  SDL_FillRect(value, NULL, BOT_RED_COLOR);
  SDL_BlitSurface(value, NULL, screen, &rect_dest);
  SDL_FreeSurface(value);
}

void bot_move(map *map, bot *in) {
  int pos;
  in->left = 0;
  in->right = 0;
  in->up = 0;
  in->down = 0;

  /* Look for free cases */
  if (in->x - 1 > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x - 1, in->y))
    in->left = 1;
  if (in->x + 1 < map->w && map_equals(map, in->x + 1, in->y, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x + 1, in->y))
    in->right = 1;
  if (in->y - 1 > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x, in->y - 1))
    in->up = 1;
  if (in->y + 1 < map->h && map_equals(map, in->x, in->y + 1, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x, in->y + 1))
    in->down = 1;

  /* Look for older choices */
  pos = bot_memory_position(in->nodes, in->x, in->y);
  if (pos >= 0) {
    bot_memory *tmp = in->nodes;
    in->left = tmp->left[pos];
    in->right = tmp->right[pos];
    in->up = tmp->up[pos];
    in->down = tmp->down[pos];
  }

  /* Look for the end */
  if (in->x - 1 > 0 && map_equals(map, in->x - 1, in->y, MAP_STOP_CASE)) {
    in->left = 0;
    //printf("LEFT\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->x--;
    in->finished = 1;
  } else if (in->x + 1 < map->w &&
             map_equals(map, in->x + 1, in->y, MAP_STOP_CASE)) {
    in->right = 0;
    //printf("RIGHT\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->x++;
    in->finished = 1;
  } else if (in->y - 1 > 0 &&
             map_equals(map, in->x, in->y - 1, MAP_STOP_CASE)) {
    in->up = 0;
    //printf("UP\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->y--;
    in->finished = 1;
  } else if (in->y + 1 < map->h &&
             map_equals(map, in->x, in->y + 1, MAP_STOP_CASE)) {
    in->down = 0;
    //printf("DOWN\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->y++;
    in->finished = 1;
  } else {
    /* MOVE */
    if (in->left) {
      in->left = 0;
      //printf("LEFT\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->x--;
    } else if (in->right) {
      in->right = 0;
      //printf("RIGHT\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->x++;
    } else if (in->up) {
      in->up = 0;
      //printf("UP\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->y--;
    } else if (in->down) {
      in->down = 0;
      //printf("DOWN\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->y++;
    } else {
      //printf("BACK\n");
      bot_pushNode(in);
      bot_popHistory(in);
    }
  }
}

void bot_free(bot *in) {
  bot_memory_free(in->nodes);
  free(in->history);
  free(in);
}

void bot_memory_free(bot_memory *in) {
  free(in->x);
  free(in->y);
  free(in->left);
  free(in->right);
  free(in->up);
  free(in->down);
  free(in);
}