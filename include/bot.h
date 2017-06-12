#ifndef BOT_H
#define BOT_H

#include "extra.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOT_RED_COLOR 0xFF0000
#define BOT_NODE_COLOR 0xFF00FF
#define BOT_USEDNODE_COLOR 0x440044

#define LEFT 3 // Valeurs totalements arbitraires
#define RIGHT 1
#define UP 0
#define DOWN 2

typedef struct bot_memory {      // Structure mémoire du robot
  int *x, *y;                    // Tableaux de positions
  int *left, *right, *up, *down; // Tableaux de directions prises
  int length;                    // Taille des Tableaux
} bot_memory;

typedef struct bot {           // Structure robot
  int x, y, wSize, hSize, dir; // Tailles et positions
  int left, right, up, down;   // Directions courantes
  bot_memory *nodes;           // Mémoire noeuds
  extra_coords *history;       // Historique positions
  int historyLength;           // Taille de l'historique
  int on_wall, pas, finished;  // Pas, fini, à coté d'ub mur et debug ( bool )
  int algorithm, debug;        // debug et algorithme selectionné
} bot;

bot *bot_init(int x, int y, int wSize, int hSize, int mute); // Initialise un
                                                             // bot

bot_memory *bot_memory_init(); // Initialise une mémoire de bot
void bot_pushNode(bot *in);    // Ajoute la position courante aux noeuds

int bot_memory_position(
    bot_memory *nodes, int x,
    int y); // Regarde l'indice du noeud contenant la position (x,y)
int bot_memory_nodesCheck(
    bot *in, int x, int y); // Regarde si la case à  été totalement exploré
int bot_nextCase_nodesCheck(
    bot *in, int x,
    int y); // Regarde si la case suivante n'a pas totalement été exploré
void bot_loadNode(bot *in); // Charge les directions prises du noeud courant

void bot_pushHistory(bot *in); // Empile la position courante
void bot_popHistory(bot *in);  // Dépile la dernière position enregistrée
int bot_historyCheck(bot *in, int x,
                     int y); // Regarde si la position est en mémoire

void bot_move(map *map, bot *in); // Fonction de gestion des algo de déplacement
void bot_move_left(bot *in);      // Avance vers la gauche
void bot_move_right(bot *in);     // Avance vers la droite
void bot_move_up(bot *in);        // Avance ver le haut
void bot_move_down(bot *in);      // Avance vers le bas
void bot_move_back(bot *in);      // Retourne en arrière
void bot_move_dir(bot *in);       // Avance dans la direction courante

void bot_checkExit(bot *in, map *map); // Regarde si on est pres de la sortie
int bot_checkWall(bot *in, map *map); // Vérifie si on est en direction d'un mur

void bot_rotate(bot *in, int new_dir); // Change la direction
void bot_rotate_to_right(bot *in);     // Tourne à droite
void bot_rotate_to_left(bot *in);      // Tourne à gauche

void bot_free(bot *in);               // Libère le robot en mémoire
void bot_memory_free(bot_memory *in); // Libère la mémoire du robot en mémoire

/* bot_move.c */
void bot_move_normal(map *map, bot *in); // Déplacement par suivi de mur
void bot_move_safe(map *map, bot *in); // Déplacement avec essai de toutes cases
void bot_move_4ways(map *map, bot *in); // Déplacement facultatif pour comparer

#endif /* BOT_H */
