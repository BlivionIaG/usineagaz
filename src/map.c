#include "map.h"

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

void map_print(map *in) {
  for (int i = 0; i < in->h; i++) {
    printf("%s\n", in->data[i]);
  }
}

void map_draw(SDL_Surface *screen, map *in) {
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

      SDL_BlitSurface(value, NULL, screen, &rect_dest);
    }
  }

  SDL_FreeSurface(value);
}

extra_coords map_getEntry(map *in) {
  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      if (in->data[i][j] == 'D') return extra_new_coords(j, i);
    }
  }

  return extra_new_coords(-1, -1);
}
extra_coords map_getExit(map *in) {
  for (int i = 0; i < in->h; i++) {
    for (int j = 0; j < in->w; j++) {
      if (in->data[i][j] == 'S') return extra_new_coords(j, i);
    }
  }

  return extra_new_coords(-1, -1);
}

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