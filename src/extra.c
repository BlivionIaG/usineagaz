#include "extra.h"

/**
 * Fragmente une chaine de caractères en fonction d'un ou plusieurs séparateurs
 * @param  outlen  Pointeur sur l'entier contenant la taille du tableau renvoyé
 * @param  c_in    Chaine de caractères à éclater
 * @param  c_param Chaine contenant les séparateurs
 * @return         Renvoie un tableau de chaines contenant les chaines séparés
 */
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

/**
 * Utilise fgets pour obtenir une ligne dans un flux, et corrige le retour à la ligne par un \0
 * @param  s      chaine de caractères contenant le resultat
 * @param  size   Taille à ne pas dépasser dans s
 * @param  stream [description]
 * @return        Retourne 1 si la fonction a réussi sinon 0
 */
int extra_fgets(char *s, int size, FILE *stream) {
  if (fgets(s, size, stream) == NULL) return 0;
  if (s[strlen(s) - 1] == '\n') s[strlen(s) - 1] = 0;

  return 1;
}

/**
 * Transforme les entiers x et y en une structure coordonnées
 * @param  x Valeur du point sur l'axe des abscisses
 * @param  y Valeur du point sur l'axe des ordonnées
 * @return   renvoie le point sous forme de coordonnées
 */
extra_coords extra_new_coords(int x, int y) {
  extra_coords out;
  out.x = x;
  out.y = y;

  return out;
}

/**
 * Génère un entier pseudo-aléatoire entre nMin et nMax (exclus)
 * @param  nMin Valeur minimale
 * @param  nMax Valeur maximale (exclue)
 * @return      Renvoie un entier pseudo-aléatoire
 */
int extra_randomInt(int nMin, int nMax) {
  return rand() % (nMax - nMin) + nMin;
}
