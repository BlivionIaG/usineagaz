#include "bot.h"

/**
 * Initialise un robot puis renvoie son adresse
 * @param  x     position sur l'axe des abscisses
 * @param  y     position sur l'axe des ordonnées
 * @param  wSize largeur du robot
 * @param  hSize hauteur du robot
 * @param  mute  option pour afficher dans le terminal la décision prise par le
 * robot (0: off, 1:on)
 * @return       Adresse du robot initialisé
 */
bot *bot_init(int x, int y, int wSize, int hSize, int mute) {
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
  out->debug = mute;
  out->on_wall = 0;
  out->algorithm = 0;

  return out;
}

/**
 * Initialise et renvoie l'adresse d'une mémoire de robot
 * @return Adresse de la mémoire de robot initialisée
 */
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

/**
 * Ajoute la position courante et les décisions à la liste de noeuds de la
 * mémoire du robot
 * @param in Robot à éditer
 */
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

/**
 * Permer d'obtenir l'indice de la liste en mémoire du noeud de la position
 * (x,y) du robot, -1 si non trouvé
 * @param  nodes Mémoire dans laquelle on cherche
 * @param  x     position sur l'axe des abscisses
 * @param  y     position sur l'axe des ordonnées
 * @return       indice de la liste du noeud contenant la position (x,y)
 */
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

/**
 * Vérifie si le robot a essayé toutes les directions de la case
 * @param  in Robot à vérifier
 * @param  x  position sur l'axe des abscisses
 * @param  y  position sur l'axe des ordonnées
 * @return    Retourne vrai si la case ne possède plus de solution
 */
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

/**
 * Charge en mémoire du robot les anciennes décisions prises sur cet emplacement
 * @param in Robot à modifier
 */
void bot_loadNode(bot *in) {
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
}

/**
 * Met dans la pile des positions précédentes la position courante
 * @param in Robot à modifier
 */
void bot_pushHistory(bot *in) {
  in->historyLength++;
  in->history =
      realloc(in->history, (in->historyLength) * sizeof(extra_coords));

  extra_coords *tmp = in->history;
  tmp[in->historyLength - 1].x = in->x;
  tmp[in->historyLength - 1].y = in->y;
}

/**
 * Récupère et enlève le dernier élément de la pile de positions
 * @param in Robot à modifier
 */
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

/**
 * Vérifier si la case (x,y) est présente dans la pile des positions précédentes
 * @param  in Robot à vérifier
 * @param  x  position sur l'axe des abscisses
 * @param  y  position sur l'axe des ordonnées
 * @return    Retourne vrai si la case est présente en mémoire
 */
int bot_historyCheck(bot *in, int x, int y) {
  extra_coords *tmp = in->history;
  for (int i = 0; i < in->historyLength; i++) {
    if (tmp[i].x == x && tmp[i].y == y)
      return 1;
  }

  return 0;
}

/**
 * Vérifie si la case suivante dans la direction du robot est totalement
 * explorée
 * @param  in Robot à vérifier
 * @param  x  position sur l'axe des abscisses
 * @param  y  position sur l'axe des ordonnées
 * @return    Retourne vrai si la case suivante dans la direction du robot
 * possèdes des directions non essayés
 */
int bot_nextCase_nodesCheck(bot *in, int x, int y) {
  switch (in->dir) {
  case LEFT:
    return bot_memory_nodesCheck(in, x - 1, y);
    break;
  case RIGHT:
    return bot_memory_nodesCheck(in, x + 1, y);
    break;
  case UP:
    return bot_memory_nodesCheck(in, x, y - 1);
    break;
  case DOWN:
    return bot_memory_nodesCheck(in, x, y + 1);
    break;
  default:
    return 0;
    break;
  }
}

/**
 * Libère de la mémoire le robot
 * @param in Robot à libérer
 */
void bot_free(bot *in) {
  bot_memory_free(in->nodes);
  free(in->history);
  free(in);
}

/**
 * Libère la mémoire du robot de la mémoire
 * @param in Mémoire à libérer
 */
void bot_memory_free(bot_memory *in) {
  free(in->x);
  free(in->y);
  free(in->left);
  free(in->right);
  free(in->up);
  free(in->down);
  free(in);
}

/**
 * Tourne le robot dans la direction précisée
 * @param in      Robot à  tourner
 * @param new_dir Direction dans laquelle on doit pointer le robot
 */
void bot_rotate(bot *in, int new_dir) {
  if (in->debug)
    printf("ROTATE :%d\n", new_dir);
  in->dir = new_dir;
  bot_pushNode(in);
}

/**
 * Tourne le robot à gauche
 * @param in Robot à tourner
 */
void bot_rotate_to_left(bot *in) {
  switch (in->dir) {
  case LEFT:
    in->dir = DOWN;
    break;
  case RIGHT:
    in->dir = UP;
    break;
  case UP:
    in->dir = LEFT;
    break;
  case DOWN:
    in->dir = RIGHT;
    break;
  default:
    break;
  }
}

/**
 * Tourne le robot à droite
 * @param in Robot à tourner
 */
void bot_rotate_to_right(bot *in) {
  switch (in->dir) {
  case LEFT:
    in->dir = UP;
    break;
  case RIGHT:
    in->dir = DOWN;
    break;
  case UP:
    in->dir = RIGHT;
    break;
  case DOWN:
    in->dir = LEFT;
    break;
  default:
    break;
  }
}

/**
 * Vérifie s'il y a une sortie à coté de lui
 * @param in  Robot à comparer
 * @param map Carte à comparer
 */
void bot_checkExit(bot *in, map *map) {
  if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_STOP_CASE)) {
    bot_move_left(in);
    in->finished = 1;
    printf("PATH FOUND !\n");
  } else if (in->x + 1 < map->w &&
             map_equals(map, in->x + 1, in->y, MAP_STOP_CASE)) {
    bot_move_right(in);
    in->finished = 1;
    printf("PATH FOUND !\n");
  } else if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_STOP_CASE)) {
    in->up = 0;
    bot_move_up(in);
    in->finished = 1;
    printf("PATH FOUND !\n");
  } else if (in->y + 1 < map->h &&
             map_equals(map, in->x, in->y + 1, MAP_STOP_CASE)) {
    bot_move_down(in);
    in->finished = 1;
    printf("PATH FOUND !\n");
  }
}

/**
 * Vérifie si le robot est devant un mur
 * @param  in  Robot à analyser
 * @param  map Carte à analyser
 * @return     renvoie 0 (faux) s'il n'y a pas de mur, sinon 1 (vrai)
 */
int bot_checkWall(bot *in, map *map) {
  switch (in->dir) { // Avance s'il peut (et n'est pas déjà passé par la case
                     // suivante)
  case LEFT:
    if (in->x > 0 && map_equals(map, in->x - 1, in->y, MAP_WALL_CASE)) {
      return 1;
    }
    break;
  case RIGHT:
    if (in->x + 1 < map->w &&
        map_equals(map, in->x + 1, in->y, MAP_WALL_CASE)) {
      return 1;
    }
    break;
  case UP:
    if (in->y > 0 && map_equals(map, in->x, in->y - 1, MAP_WALL_CASE)) {
      return 1;
    }
    break;
  case DOWN:
    if (in->y + 1 < map->h &&
        map_equals(map, in->x, in->y + 1, MAP_WALL_CASE)) {
      return 1;
    }
    break;
  default:
    break;
  }
  return 0;
}
