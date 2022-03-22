#include <assert.h>
#include <stdlib.h>
#include "points.h"

/**
   Renvoie l'orientation de p par rapport aux deux points précédent qui forme un vecteur \vec({pMoins2}{pMoins1})
   Si retour > 0, p est à gauche.
   Si retour = 0, p est sur la droite qui découle du vecteur.
   Si retour < 0, p est à droite.
*/
double Point_orientation(Point pMoins2, Point pMoins1, Point p)
{
  return (pMoins1.x - pMoins2.x) * (p.y - pMoins2.y) - (pMoins1.y - pMoins2.y) * (p.x - pMoins2.x);
}

void TabPoints_init(TabPoints *tab)
{
  tab->taille = 100;
  tab->nb = 0;
  tab->points = (Point *)malloc(tab->taille * sizeof(Point));
}

void TabPoints_ajoute(TabPoints *tab, Point p)
{
  if (tab->nb >= tab->taille)
    TabPoints_agrandir(tab);
  tab->points[tab->nb++] = p;
}

void TabPoints_set(TabPoints *tab, int i, Point p)
{
  assert(i < tab->nb);
  tab->points[i] = p;
}

Point TabPoints_get(TabPoints *tab, int i)
{
  assert(i < tab->nb);
  return tab->points[i];
}

int TabPoints_nb(TabPoints *tab)
{
  return tab->nb;
}

void TabPoints_termine(TabPoints *tab)
{
  if (tab->points != NULL)
    free(tab->points);
  tab->taille = 0;
  tab->nb = 0;
  tab->points = NULL;
}

void TabPoints_agrandir(TabPoints *tab)
{
  tab->points = (Point *)realloc(tab->points, tab->taille * 2 * sizeof(Point));
  tab->taille *= 2;
}

Point *TabPoints_min(TabPoints *tab)
{
  Point *min = tab->points;
  for (int i = 0; i < tab->nb; i++)
  {
    if (tab->points[i].y < min->y)
      min = &tab->points[i];
  }
  return min;
}

int equals(Point p1, Point p2)
{
  if (p1.y == p2.y && p1.x == p2.x)
    return 1;
  return 0;
}