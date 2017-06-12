#ifndef EXTRA_H
#define EXTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct extra_coords {
  int x, y;
} extra_coords; // Structure de coordonnés

char **extra_split(
    int *outlen, const char *c_in,
    const char *c_param); // Fonction pour éclater en fonction d'un paramètre

int extra_fgets(char *s, int size, FILE *stream); // Fonction pour obtenir une
                                                  // ligne de texte d'un flux
                                                  // (correction \n -> 0
                                                  // automatique)

extra_coords
extra_new_coords(int x, int y); // Transforme les points x et y en coordonnées

int extra_randomInt(
    int nMin,
    int nMax); // Génère un entier pseudo aléatoire entre nMin et nMax (exclus)

#endif /* EXTRA_H */
