#include "extra.h"

char **extra_split(int *outlen, const char *c_in, const char *c_param) {
  char **out = NULL;
  *outlen = 0;

  char *token = NULL;
  char *rest = malloc((strlen(c_in) + 1) * sizeof(char));
  char *restBak = rest;
  strcpy(rest, c_in);

  while ((token = strtok_r(rest, c_param, &rest))) {
    (*outlen)++;
    out = realloc(out, sizeof(char *) * (*outlen));
    out[(*outlen) - 1] = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(out[(*outlen) - 1], token);
  }

  free(restBak);

  return out;
}

int extra_fgets(char *s, int size, FILE *stream) {
  if (fgets(s, size, stream) == NULL) return 0;
  if (s[strlen(s) - 1] == '\n') s[strlen(s) - 1] = 0;

  return 1;
}

extra_coords extra_new_coords(int x, int y) {
  extra_coords out;
  out.x = x;
  out.y = y;

  return out;
}

int extra_randomInt(int nMin, int nMax) {
  return rand() % (nMax - nMin) + nMin;
}