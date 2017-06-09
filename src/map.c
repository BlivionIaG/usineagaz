#include "map.h"

/**
 * Charge une carte dans une structure map et renvoie son addresse
 * @param  path Emplacement du fichier à charger
 * @param  size Hauteur indiquée de la carte
 * @return      Renvoie un pointeur sur une structure map contenant une carte
 */
map *map_load(char *path, int size) {
  FILE *labyrinthe = fopen(path, "r");  // Open maze file
  if (labyrinthe == NULL) {             // If failed, exit
    printf("ERREUR donne un fichier qui existe !!!\n");
    return NULL;
  }

  map *out = malloc(sizeof(map));

  char *lecture = malloc((MAX_LAB_WIDTH + 1) * sizeof(char));
  out->w = 0;

  if (!extra_fgets(lecture, MAX_LAB_WIDTH, labyrinthe)) {
    printf("ERREUR le fichier est invalide !\n");
    return NULL;
  } else {
    int tmpLen = 0;
    char **tmp = extra_split(&tmpLen, lecture, ":");
    out->w = atoi(tmp[0]);
    out->h = atoi(tmp[1]);

    printf("largeur:%d\n", out->w);
    printf("hauteur:%d\n", out->h);

    for (int i = 0; i < tmpLen; i++) {
      free(tmp[i]);
    }
    free(tmp);
  }

  out->data = NULL;
  int terrainLength = 0;

  while (extra_fgets(lecture, MAX_LAB_WIDTH,
                     labyrinthe)) {  // out->w + 1 trop petit
    terrainLength++;
    out->data = realloc(out->data, terrainLength * sizeof(char *));
    out->data[terrainLength - 1] = malloc((MAX_LAB_WIDTH + 1) * sizeof(char));
    strcpy(out->data[terrainLength - 1], lecture);
  }
  printf("terrainLength:%d\n", terrainLength);
  if (terrainLength != out->h) {
    printf("ERREUR mauvaise taille indiquée dans le fichier, correction...\n");
    out->h = terrainLength;
  }

  out->size = size;
  fclose(labyrinthe);
  free(lecture);

  return out;
}

/**
 * Affiche la carte dans le terminal
 * @param in Carte à afficher
 */
void map_print(map *in) {
  for (int i = 0; i < in->h; i++) {
    printf("%s\n", in->data[i]);
  }
}

/**
 * Retourne la position du point de départ de la carte
 * @param  in Carte à analyser
 * @return    Renvoie la position du point de départ sous forme de coordonnées, négatives si non trouvés
 */
extra_coords map_getEntry(map *in) {
  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      if (in->data[i][j] == 'D') return extra_new_coords(j, i);
    }
  }

  return extra_new_coords(-1, -1);
}

/**
 * Retourne la position du point d'arrivé de la carte
 * @param  in Carte à analyser
 * @return    Renvoie la position du point d'arrivé sous forme de coordonnées, négatives si non trouvés
 */
extra_coords map_getExit(map *in) {
  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      if (in->data[i][j] == 'S') return extra_new_coords(j, i);
    }
  }

  return extra_new_coords(-1, -1);
}

/**
 * Vérifie si le caractère c est le meme que celui de la carte à la position (x,y)
 * @param  in Carte à analyser
 * @param  x  Position sur l'axe des abscisses
 * @param  y  Position sur l'axe des ordonnés
 * @param  c  Caractère à comparer
 * @return    Renvoie 1 (vrai) si identique, sinon 0 (faux)
 */
int map_equals(map *in, int x, int y, char c) {
  if (in->data[y][x] == c) return 1;
  return 0;
}


void map_free(map *in) {
  for (int i = 0; i < in->h; i++) {
    free(in->data[i]);
  }
  free(in->data);
  free(in);
}
