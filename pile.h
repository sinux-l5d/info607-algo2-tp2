/**
    doit contenir les structures et les prototypes
    des fonctions de gestion des piles
**/


#ifndef _PILE_H_
#define _PILE_H_
 
#include "points.h"
 
typedef struct SPilePoint {
  int taille;
  int nb;
  Point* points;
} PilePoints;
 
void  PilePoints_init( PilePoints* pile, int taille);
int   PilePoints_estVide( PilePoints* pile );
void  PilePoints_empile( PilePoints* pile, Point p );
void  PilePoints_depile( PilePoints* pile );
Point PilePoints_sommet( PilePoints* pile );
Point PilePoints_deuxiemeSommet( PilePoints* pile );
void  PilePoints_agrandir( PilePoints* pile );
void  PilePoints_termine( PilePoints* pile );
int   PilePoints_nb( PilePoints* pile );
Point PilePoints_get( PilePoints* pile, int idx );
 
#endif
