#include "bot.h"

void bot_move(map *map, bot *in) {
  if (in->algorithm) {
    bot_move_safe(map, in);
  } else {
    bot_move_normal(map, in);
  }
}

void bot_move_left(bot *in) {
  in->pas++;

  in->left = 0;
  if (in->debug) printf("LEFT\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->x--;
}

void bot_move_right(bot *in) {
  in->pas++;

  in->right = 0;
  if (in->debug) printf("RIGHT\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->x++;
}

void bot_move_up(bot *in) {
  in->pas++;

  in->up = 0;
  if (in->debug) printf("UP\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->y--;
}

void bot_move_down(bot *in) {
  in->pas++;

  in->down = 0;
  if (in->debug) printf("DOWN\n");
  bot_pushNode(in);
  bot_pushHistory(in);
  in->y++;
}

void bot_move_back(bot *in) {
  in->pas++;

  if (in->debug) printf("BACK\n");
  in->left = 0;
  in->right = 0;
  in->up = 0;
  in->down = 0;
  bot_pushNode(in);
  bot_popHistory(in);
}

void bot_move_normal(
    map *map,
    bot *in) {  // si mur -> tourne à gauche sinon avance et tourne à droite
  bot_checkExit(in, map);
  if (in->finished) return;

  if (bot_historyCheck(in, in->x, in->y)) {
    in->algorithm = 1;
    printf("NORMAL -> SAFE\n");
    return;
  }

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

  if (!in->on_wall) {
    switch (in->dir) {  // Avance s'il peut (et n'est pas déjà passé par la case
                        // suivante)
      case LEFT:
        if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE)) {
          bot_move_left(in);
          return;
        }
        break;
      case RIGHT:
        if (in->x + 1 < map->w &&
            map_equals(map, in->x + 1, in->y, MAP_FREE_CASE)) {
          bot_move_right(in);
          return;
        }
        break;
      case UP:
        if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE)) {
          bot_move_up(in);
          return;
        }
        break;
      case DOWN:
        if (in->y + 1 < map->h &&
            map_equals(map, in->x, in->y + 1, MAP_FREE_CASE)) {
          bot_move_down(in);
          return;
        }
        break;
      default:
        break;
    }
    in->on_wall = 1;
    if (in->debug) printf("WALL FOUND !\n");
    bot_move(map, in);
  } else {
    switch (in->dir) {  // Avance s'il peut (et n'est pas déjà passé par la case
                        // suivante)
      case LEFT:
        if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE)) {
          bot_move_left(in);
          bot_rotate_to_right(in);
          return;
        }
        break;
      case RIGHT:
        if (in->x + 1 < map->w &&
            map_equals(map, in->x + 1, in->y, MAP_FREE_CASE)) {
          bot_move_right(in);
          bot_rotate_to_right(in);
          return;
        }
        break;
      case UP:
        if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE)) {
          bot_move_up(in);
          bot_rotate_to_right(in);
          return;
        }
        break;
      case DOWN:
        if (in->y + 1 < map->h &&
            map_equals(map, in->x, in->y + 1, MAP_FREE_CASE)) {
          bot_move_down(in);
          bot_rotate_to_right(in);
          return;
        }
        break;
      default:
        break;
    }
    bot_rotate_to_left(in);
  }
}

void bot_move_safe(map *map, bot *in) {
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

  if (bot_nextCase_nodesCheck(in, in->x, in->y) &&
      (in->on_wall = bot_checkWall(in, map))) {
    in->algorithm = 0;
    printf("SAFE -> NORMAL\n");
  }

  switch (in->dir) {  // Avance s'il peut (et n'est pas déjà passé par la case
                      // suivante)
    case LEFT:
      if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE) &&
          in->left) {
        bot_move_left(in);
        return;
      } else {
        in->left = 0;
        if (in->x + 1 < map->w && bot_historyCheck(in, in->x + 1, in->y)) {
          in->right = 0;
        }
      }
      break;
    case RIGHT:
      if (in->x + 1 < map->w &&
          map_equals(map, in->x + 1, in->y, MAP_FREE_CASE) && in->right) {
        bot_move_right(in);
        return;
      } else {
        in->right = 0;
        if (in->x > 0 && bot_historyCheck(in, in->x - 1, in->y)) {
          in->left = 0;
        }
      }
      break;
    case UP:
      if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE) &&
          in->up) {
        bot_move_up(in);
        return;
      } else {
        in->up = 0;
        if (in->y + 1 < map->h && bot_historyCheck(in, in->x, in->y + 1)) {
          in->down = 0;
        }
      }
      break;
    case DOWN:
      if (in->y + 1 < map->h &&
          map_equals(map, in->x, in->y + 1, MAP_FREE_CASE) && in->down) {
        bot_move_down(in);
        return;
      } else {
        in->down = 0;
        if (in->y > 0 && bot_historyCheck(in, in->x, in->y - 1)) {
          in->up = 0;
        }
      }
      break;
    default:
      break;
  }

  if (in->left && !bot_memory_nodesCheck(in, in->x - 1, in->y)) {
    bot_rotate(in, LEFT);
    return;
  } else if (in->up && !bot_memory_nodesCheck(in, in->x, in->y - 1)) {
    bot_rotate(in, UP);
    return;
  } else if (in->right && !bot_memory_nodesCheck(in, in->x + 1, in->y)) {
    bot_rotate(in, RIGHT);
    return;
  } else if (in->down && !bot_memory_nodesCheck(in, in->x, in->y + 1)) {
    bot_rotate(in, DOWN);
    return;
  }

  bot_move_back(in);
}

void bot_move_4ways(map *map, bot *in) {
  int pos;
  in->left = 0;
  in->right = 0;
  in->up = 0;
  in->down = 0;

  // Look for free cases
  if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x - 1, in->y))
    in->left = 1;
  if (in->x + 1 < map->w && map_equals(map, in->x + 1, in->y, MAP_FREE_CASE) &&
      !bot_historyCheck(in, in->x + 1, in->y))
    in->right = 1;
  if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_FREE_CASE) &&
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
  if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_STOP_CASE)) {
    in->left = 0;
    // printf("LEFT\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->x--;
    in->finished = 1;
  } else if (in->x + 1 < map->w &&
             map_equals(map, in->x + 1, in->y, MAP_STOP_CASE)) {
    in->right = 0;
    // printf("RIGHT\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->x++;
    in->finished = 1;
  } else if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_STOP_CASE)) {
    in->up = 0;
    // printf("UP\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->y--;
    in->finished = 1;
  } else if (in->y + 1 < map->h &&
             map_equals(map, in->x, in->y + 1, MAP_STOP_CASE)) {
    in->down = 0;
    // printf("DOWN\n");
    bot_pushNode(in);
    bot_pushHistory(in);
    in->y++;
    in->finished = 1;
  } else {
    // MOVE
    if (in->left) {
      in->left = 0;
      // printf("LEFT\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->x--;
    } else if (in->right) {
      in->right = 0;
      // printf("RIGHT\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->x++;
    } else if (in->up) {
      in->up = 0;
      // printf("UP\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->y--;
    } else if (in->down) {
      in->down = 0;
      // printf("DOWN\n");
      bot_pushNode(in);
      bot_pushHistory(in);
      in->y++;
    } else {
      // printf("BACK\n");
      bot_pushNode(in);
      bot_popHistory(in);
    }
  }
}