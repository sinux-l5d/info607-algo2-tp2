#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>
#include <time.h>
#include "points.h"
#include "quicksort.h"
#include "pile.h"

//-----------------------------------------------------------------------------
// Déclaration des types
//-----------------------------------------------------------------------------
/**
   Le contexte contient les informations utiles de l'interface pour
   les algorithmes de géométrie algorithmique.
*/
typedef struct SContexte
{
  int width;
  int height;
  GtkWidget *drawing_area;
  TabPoints P;
  PilePoints pile;
  int indiceHaut;

  double temps_exec;

  GtkWidget *entry;
  GtkWidget *label;
  GtkWidget *label_nb_points;
  GtkWidget *label_nb_sommets;
  GtkWidget *label_temps_exec;
} Contexte;

//-----------------------------------------------------------------------------
// Déclaration des fonctions
//-----------------------------------------------------------------------------
/**
   Crée l'interface graphique en fonction du contexte \a pCtxt.
*/
GtkWidget *creerIHM(Contexte *pCtxt);

/**
   c'est la réaction principale qui va redessiner tout.
*/
gboolean on_draw(GtkWidget *widget, GdkEventExpose *event, gpointer data);

/**
   Renvoie un entier depuis un GtkEntry
*/
int getNbFrom(GtkWidget *entry);

/**
   Génère un certain nombre de points distribués aléatoirement dans le
   disque unité et les ajoute au contexte.
*/
gboolean diskRandom(GtkWidget *widget, gpointer data);

/**
   Génère un certain nombre de points distribués aléatoirement dans le
   losange et les ajoute au contexte.
*/
gboolean losangeRandom(GtkWidget *widget, gpointer data);

gboolean convexe_graham(GtkWidget *widget, gpointer data);

/**
   Fait la conversion coordonnées réelles de \a p vers coordonnées de la zone de dessin.
   @param p le point en entrée
   @param pCtxt le contexte de l'IHM
   @return ses coordonnées dans la zone de dessin.
*/
Point point2DrawingArea(Point p, Contexte *pCtxt);

/**
   Affiche un point \a p dans une zone de dessin cairo \a cr comme un disque.

   @param cr le contexte CAIRO pour dessiner dans une zone de dessin.
   @param p un point dans la zone de dessin.
 */
void drawPoint(cairo_t *cr, Point p);

void drawLine(cairo_t *cr, Point p, Point q);

gboolean convexe_jarvis(GtkWidget *widget, gpointer data);
int get_min_angle_haut(TabPoints *tab, Point p, Point repere);

//-----------------------------------------------------------------------------
// Programme principal
//-----------------------------------------------------------------------------
int main(int argc,
         char *argv[])
{
  Contexte context;
  TabPoints_init(&context.P);
  PilePoints_init(&context.pile, 100);

  /* Passe les arguments à GTK, pour qu'il extrait ceux qui le concernent. */
  gtk_init(&argc, &argv);

  /* Crée une fenêtre. */
  creerIHM(&context);

  /* Rentre dans la boucle d'événements. */
  gtk_main();
  return 0;
}

void drawLine(cairo_t *cr, Point p, Point q)
{
  cairo_move_to(cr, p.x, p.y);
  cairo_line_to(cr, q.x, q.y);
  cairo_stroke(cr);
}

// c'est la réaction principale qui va redessiner tout.
gboolean on_draw(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  Contexte *pCtxt = (Contexte *)data;
  TabPoints *ptrP = &(pCtxt->P);
  PilePoints *pile = &(pCtxt->pile);
  char str[100];
  sprintf(str, "%d", pCtxt->P.nb);
  gtk_label_set_text(GTK_LABEL(pCtxt->label), str);
  // c'est la structure qui permet d'afficher dans une zone de dessin
  // via Cairo
  GdkWindow *window = gtk_widget_get_window(widget);
  cairo_region_t *cairoRegion = cairo_region_create();
  GdkDrawingContext *drawingContext = gdk_window_begin_draw_frame(window, cairoRegion);
  cairo_t *cr = gdk_drawing_context_get_cairo_context(drawingContext);
  cairo_set_source_rgb(cr, 1, 1, 1); // choisit le blanc.
  cairo_paint(cr);                   // remplit tout dans la couleur choisie.

  // Affiche tous les points en bleu.
  cairo_set_source_rgb(cr, 0, 0, 1);
  for (int i = 0; i < TabPoints_nb(ptrP); ++i)
    drawPoint(cr, point2DrawingArea(TabPoints_get(ptrP, i), pCtxt));

  // printf("nb : %i\n",PilePoints_nb(pile));
  // if (!PilePoints_estVide(pile))
  // PilePoints_nb( pile )-1

  // Affiche l'enveloppe
  // for (int i = 0; i < TabPoints_nb(ptrP) - 1; ++i)
  //   drawLine(cr, point2DrawingArea(TabPoints_get(ptrP, i), pCtxt), point2DrawingArea(TabPoints_get(ptrP, i + 1), pCtxt));

  // if (TabPoints_nb(ptrP) > 0)
  //   drawLine(cr, point2DrawingArea(TabPoints_get(ptrP, 0), pCtxt), point2DrawingArea(TabPoints_get(ptrP, TabPoints_nb(ptrP) - 1), pCtxt));

  // Affiche l'enveloppe
  for (int i = 0; i < PilePoints_nb(pile) - 1; ++i)
    drawLine(cr, point2DrawingArea(PilePoints_get(pile, i), pCtxt), point2DrawingArea(PilePoints_get(pile, i + 1), pCtxt));

  if (PilePoints_nb(pile) > 0)
    drawLine(cr, point2DrawingArea(PilePoints_get(pile, 0), pCtxt), point2DrawingArea(PilePoints_get(pile, PilePoints_nb(pile) - 1), pCtxt));

  // printf("indiceHaut: %i\n", pCtxt->indiceHaut);
  // if (pCtxt->indiceHaut != -1)
  //   drawLine(cr, point2DrawingArea(TabPoints_get(ptrP, 0), pCtxt), point2DrawingArea(TabPoints_get(ptrP, pCtxt->indiceHaut), pCtxt));

  // MAJ des labels
  char buffer[20];

  sprintf(buffer, "%d points", TabPoints_nb(ptrP));
  gtk_label_set_text(GTK_LABEL(pCtxt->label_nb_points), buffer);

  sprintf(buffer, "%d sommets", PilePoints_nb(pile));
  gtk_label_set_text(GTK_LABEL(pCtxt->label_nb_sommets), buffer);

  sprintf(buffer, "%lf ms", pCtxt->temps_exec);
  gtk_label_set_text(GTK_LABEL(pCtxt->label_temps_exec), buffer);

  // On a fini, on peut détruire la structure.
  gdk_window_end_draw_frame(window, drawingContext);
  // cleanup
  cairo_region_destroy(cairoRegion);

  return TRUE;
}

Point point2DrawingArea(Point p, Contexte *pCtxt)
{
  Point q;
  q.x = (p.x + 1.0) / 2.0 * pCtxt->width;
  q.y = (1.0 - p.y) / 2.0 * pCtxt->height;
  return q;
}

void drawPoint(cairo_t *cr, Point p)
{
  cairo_arc(cr, p.x, p.y, 1.5, 0., 2.0 * 3.14159626);
  cairo_fill(cr);
}

/// Charge l'image donnée et crée l'interface.
GtkWidget *creerIHM(Contexte *pCtxt)
{
  GtkWidget *window;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *button_quit;
  GtkWidget *button_disk_random;
  GtkWidget *entry;
  GtkWidget *label;
  GtkWidget *button_losange;
  GtkWidget *button_graham;
  GtkWidget *label_nb_points;
  GtkWidget *label_nb_sommets;
  GtkWidget *label_temps_exec;
  GtkWidget *button_jarvis;

  /* Crée une fenêtre. */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  // Crée un conteneur horizontal box.
  hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  // Crée deux conteneurs vertical box.
  vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  entry = gtk_entry_new();
  pCtxt->entry = entry;
  gtk_entry_set_text(GTK_ENTRY(entry), "0");

  char str[100];
  sprintf(str, "%d", pCtxt->P.nb);
  label = gtk_label_new(str);
  pCtxt->label = label;
  pCtxt->indiceHaut = -1;
  // Crée une zone de dessin
  pCtxt->drawing_area = gtk_drawing_area_new();
  pCtxt->width = 750;
  pCtxt->height = 750;
  gtk_widget_set_size_request(pCtxt->drawing_area, pCtxt->width, pCtxt->height);
  // Crée le pixbuf source et le pixbuf destination
  gtk_container_add(GTK_CONTAINER(hbox1), pCtxt->drawing_area);
  // ... votre zone de dessin s'appelle ici "drawing_area"
  g_signal_connect(G_OBJECT(pCtxt->drawing_area), "draw",
                   G_CALLBACK(on_draw), pCtxt);

  // Rajoute le 2eme vbox dans le conteneur hbox (pour mettre les boutons sélecteur de points
  gtk_container_add(GTK_CONTAINER(hbox1), vbox2);
  // Crée les boutons de sélection "source"/"destination".
  button_disk_random = gtk_button_new_with_label("Points aléatoires dans disque");
  button_losange = gtk_button_new_with_label("Points aléatoire dans un losange");
  button_graham = gtk_button_new_with_label("convexe graham");
  button_jarvis = gtk_button_new_with_label("covexe Jarvis");

  label_nb_points = gtk_label_new("0 points");
  label_nb_sommets = gtk_label_new("0 sommets");
  label_temps_exec = gtk_label_new("0 ms");
  pCtxt->label_nb_points = label_nb_points;
  pCtxt->label_nb_sommets = label_nb_sommets;
  pCtxt->label_temps_exec = label_temps_exec;

  // Connecte la réaction gtk_main_quit à l'événement "clic" sur ce bouton.
  g_signal_connect(button_disk_random, "clicked",
                   G_CALLBACK(diskRandom),
                   pCtxt);
  g_signal_connect(button_losange, "clicked",
                   G_CALLBACK(losangeRandom),
                   pCtxt);
  g_signal_connect(button_graham, "clicked",
                   G_CALLBACK(convexe_graham),
                   pCtxt);
  g_signal_connect(button_jarvis, "clicked",
                   G_CALLBACK(convexe_jarvis),
                   pCtxt);

  gtk_container_add(GTK_CONTAINER(vbox2), button_disk_random);
  gtk_container_add(GTK_CONTAINER(vbox2), button_losange);
  gtk_container_add(GTK_CONTAINER(vbox2), label);
  gtk_container_add(GTK_CONTAINER(vbox2), entry);
  gtk_container_add(GTK_CONTAINER(vbox2), button_graham);
  gtk_container_add(GTK_CONTAINER(vbox2), label_nb_points);
  gtk_container_add(GTK_CONTAINER(vbox2), label_nb_sommets);
  gtk_container_add(GTK_CONTAINER(vbox2), label_temps_exec);
  gtk_container_add(GTK_CONTAINER(vbox2), button_jarvis);

  // Crée le bouton quitter.
  button_quit = gtk_button_new_with_label("Quitter");
  // Connecte la réaction gtk_main_quit à l'événement "clic" sur ce bouton.
  g_signal_connect(button_quit, "clicked",
                   G_CALLBACK(gtk_main_quit),
                   NULL);
  // Rajoute tout dans le conteneur vbox.
  gtk_container_add(GTK_CONTAINER(vbox1), hbox1);
  gtk_container_add(GTK_CONTAINER(vbox1), button_quit);
  // Rajoute la vbox  dans le conteneur window.
  gtk_container_add(GTK_CONTAINER(window), vbox1);

  // Rend tout visible
  gtk_widget_show_all(window);

  return window;
}

int getNbFrom(GtkWidget *entry)
{
  const char *txt = gtk_entry_get_text(GTK_ENTRY(entry));
  return atoi(txt);
}

gboolean diskRandom(GtkWidget *widget, gpointer data)
{
  Contexte *pCtxt = (Contexte *)data;
  TabPoints *ptrP = &(pCtxt->P);
  int nbPoints = getNbFrom(pCtxt->entry);
  for (int i = 0; i < nbPoints; ++i)
  {
    Point p;
    do
    {
      p.x = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
      p.y = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
    } while ((p.x * p.x + p.y * p.y) > 1.0);
    TabPoints_ajoute(ptrP, p);
  }
  gtk_widget_queue_draw(pCtxt->drawing_area);

  return TRUE;
}

gboolean losangeRandom(GtkWidget *widget, gpointer data)
{
  Contexte *pCtxt = (Contexte *)data;
  TabPoints *ptrP = &(pCtxt->P);
  int nbPoints = getNbFrom(pCtxt->entry);
  for (int i = 0; i < nbPoints; ++i)
  {
    Point p;
    p.x = 2.0 * (rand() / (double)RAND_MAX) - 1.0;
    p.y = 2.0 * (rand() / (double)RAND_MAX) - 1.0;

    Point pLosange;
    pLosange.x = p.x * (sqrt(2) / 4) + p.y * (sqrt(2) / 4);
    pLosange.y = p.x * -(sqrt(2) / 4) + p.y * (sqrt(2) / 4);

    TabPoints_ajoute(ptrP, pLosange);
  }
  gtk_widget_queue_draw(pCtxt->drawing_area);

  return TRUE;
}

gboolean convexe_graham(GtkWidget *widget, gpointer data)
{
  // On veut le temps d'éxécution de la fonction
  struct timespec myTimerStart;
  clock_gettime(CLOCK_REALTIME, &myTimerStart);

  Contexte *pCtxt = (Contexte *)data;
  TabPoints *ptrP = &(pCtxt->P);
  quicksort(ptrP);

  PilePoints *pile = &(pCtxt->pile);
  PilePoints_init(pile, 100); // réinitialise la pile si elle n'est pas vide
  PilePoints_empile(pile, ptrP->points[0]);
  PilePoints_empile(pile, ptrP->points[1]);

  // puisque origine fixe, nombre elem changé ?
  for (int i = 2; i < TabPoints_nb(ptrP); i++)
  {
    while (Point_orientation(PilePoints_deuxiemeSommet(pile), PilePoints_sommet(pile), ptrP->points[i]) <= 0.0)
    {
      PilePoints_depile(pile);
    }
    PilePoints_empile(pile, ptrP->points[i]);
  }

  // Stoppe l'horloge
  struct timespec current;
  clock_gettime(CLOCK_REALTIME, &current); // Linux gettime
  double t = ((current.tv_sec - myTimerStart.tv_sec) * 1000 +
              (current.tv_nsec - myTimerStart.tv_nsec) / 1000000.0);

  pCtxt->temps_exec = t;

  gtk_widget_queue_draw(pCtxt->drawing_area);

  return TRUE;
}

gboolean convexe_jarvis(GtkWidget *widget, gpointer data)
{
  Contexte *pCtxt = (Contexte *)data;
  TabPoints *ptrP = &(pCtxt->P);

  // On veut le temps d'éxécution de la fonction
  struct timespec myTimerStart;
  clock_gettime(CLOCK_REALTIME, &myTimerStart);

  PilePoints *pile = &(pCtxt->pile);
  PilePoints_init(pile, 100); // réinitialise la pile si elle n'est pas vide

  Point p = *TabPoints_min(ptrP);
  Point p0 = p;
  Point pointAGauche;
  int count = 0;
  // marche de jarvis
  do
  {
    PilePoints_empile(pile, p);
    pointAGauche = TabPoints_get(ptrP, 0);
    for (int i = 1; i < TabPoints_nb(ptrP); i++)
    {
      if (equals(pointAGauche, p) || Point_orientation(p, pointAGauche, TabPoints_get(ptrP, i)) > 0.0)
      {
        pointAGauche = TabPoints_get(ptrP, i);
      }
    }
    p = pointAGauche;
    count++;

  } while (!equals(p, p0));

  // Stoppe l'horloge
  struct timespec current;
  clock_gettime(CLOCK_REALTIME, &current); // Linux gettime
  double t = ((current.tv_sec - myTimerStart.tv_sec) * 1000 +
              (current.tv_nsec - myTimerStart.tv_nsec) / 1000000.0);

  pCtxt->temps_exec = t;

  gtk_widget_queue_draw(pCtxt->drawing_area);

  return TRUE;
}
