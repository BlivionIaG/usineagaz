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
  out->dir = LEFT;
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

int bot_memory_nodesCheck(bot *in, int x, int y) {
  int pos = bot_memory_position(in->nodes, x, y);
  if (pos < 0) {
    return 0;
  } else {
    bot_memory *tmp = in->nodes;
    if (tmp->left[pos] + tmp->right[pos] + tmp->up[pos] + tmp->down[pos]) {
      return 0;
    } else {
      return 1;
    }
  }
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

void bot_debugDraw(SDL_Surface *screen, bot *in) {
  SDL_Rect rect_dest;  // Rectangle destination
  rect_dest.x = in->x * in->wSize;
  rect_dest.y = in->y * in->hSize;
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
    SDL_BlitSurface(value, NULL, screen, &rect_dest);
  }

  SDL_FillRect(value, NULL, BOT_RED_COLOR);
  SDL_BlitSurface(value, NULL, screen, &rect_dest);

  SDL_FreeSurface(value);
}

void bot_move(map *map, bot *in) {
  bot_checkExit(in, map);
  if (in->finished) return;

  int pos = bot_memory_position(in->nodes, in->x, in->y);

  if (pos < 0) {
    in->left = 1;
    in->right = 1;
    in->up = 1;
    in->down = 1;
  } else {
    bot_memory *tmp = in->nodes;
    in->left = tmp->left[pos];
    in->right = tmp->right[pos];
    in->up = tmp->up[pos];
    in->down = tmp->down[pos];
  }

  switch (in->dir) {  // Avance s'il peut (et n'est pas déjà passé par la case
                      // suivante)
    case LEFT:
      if (in->x - 1 > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE) &&
          in->left) {
        bot_move_left(in);
        return;
      } else {
        in->left = 0;
      }
      break;
    case RIGHT:
      if (in->x + 1 < map->w &&
          map_equals(map, in->x + 1, in->y, MAP_FREE_CASE) && in->right) {
        bot_move_right(in);
        return;
      } else {
        in->right = 0;
      }
      break;
    case UP:
      if (in->y - 1 > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE) &&
          in->up) {
        bot_move_up(in);
        return;
      } else {
        in->up = 0;
      }
      break;
    case DOWN:
      if (in->y + 1 < map->h &&
          map_equals(map, in->x, in->y + 1, MAP_FREE_CASE) && in->down) {
        bot_move_down(in);
        return;
      } else {
        in->down = 0;
      }
      break;
    default:
      break;
  }

  if (in->left && !bot_memory_nodesCheck(in, in->x - 1, in->y)) {
    bot_rotate(in, LEFT);
    return;
  } else if (in->right && !bot_memory_nodesCheck(in, in->x + 1, in->y)) {
    bot_rotate(in, RIGHT);
    return;
  } else if (in->up && !bot_memory_nodesCheck(in, in->x, in->y - 1)) {
    bot_rotate(in, UP);
    return;
  } else if (in->down && !bot_memory_nodesCheck(in, in->x, in->y + 1)) {
    bot_rotate(in, DOWN);
    return;
  }

  bot_move_back(in);
}

/*
void bot_move(map *map, bot *in) {
  int pos;
  in->left = 0;
  in->right = 0;
  in->up = 0;
  in->down = 0;

  // Look for free cases
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

  // Look for older choices
  pos = bot_memory_position(in->nodes, in->x, in->y);
  if (pos >= 0) {
    bot_memory *tmp = in->nodes;
    in->left = tmp->left[pos];
    in->right = tmp->right[pos];
    in->up = tmp->up[pos];
    in->down = tmp->down[pos];
  }

  // Look for the end
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
    // MOVE
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
}*/

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

void bot_move_left(bot *in) {
  in->left = 0;
  printf("LEFT\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->x--;
}

void bot_move_right(bot *in) {
  in->right = 0;
  printf("RIGHT\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->x++;
}

void bot_move_up(bot *in) {
  in->up = 0;
  printf("UP\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->y--;
}

void bot_move_down(bot *in) {
  in->down = 0;
  printf("DOWN\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->y++;
}

void bot_move_back(bot *in) {
  printf("BACK\n");
  in->left = 0;
  in->right = 0;
  in->up = 0;
  in->down = 0;
  bot_pushNode(in);
  bot_popHistory(in);
}

void bot_rotate(bot *in, int new_dir) {
  printf("ROTATE :%d\n", new_dir);
  in->dir = new_dir;
  switch (in->dir) {
    case LEFT:
      //in->left = 0;
      break;
    case RIGHT:
      //in->right = 0;
      break;
    case UP:
     //in->up = 0;
      break;
    case DOWN:
      //in->down = 0;
      break;
    default:
      break;
  }
  bot_pushNode(in);
}

void bot_reverseDir(bot *in) {
  switch (in->dir) {
    case LEFT:
      in->dir = RIGHT;
      break;
    case RIGHT:
      in->dir = LEFT;
      break;
    case UP:
      in->dir = DOWN;
      break;
    case DOWN:
      in->dir = UP;
      break;
    default:
      break;
  }
}

void bot_checkExit(bot *in, map *map) {
  if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_STOP_CASE)) {
    bot_move_left(in);
    in->finished = 1;
    printf("PATH FOUNDED !\n");
  } else if (in->x < map->w &&
             map_equals(map, in->x + 1, in->y, MAP_STOP_CASE)) {
    bot_move_right(in);
    in->finished = 1;
    printf("PATH FOUNDED !\n");
  } else if (in->y > 0 &&
             map_equals(map, in->x, in->y - 1, MAP_STOP_CASE)) {
    in->up = 0;
    bot_move_up(in);
    in->finished = 1;
    printf("PATH FOUNDED !\n");
  } else if (in->y < map->h &&
             map_equals(map, in->x, in->y + 1, MAP_STOP_CASE)) {
    bot_move_down(in);
    in->finished = 1;
    printf("PATH FOUNDED !\n");
  }
}