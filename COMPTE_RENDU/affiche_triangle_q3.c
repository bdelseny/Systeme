#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "calcul_triangle.h"
#include "boite_chiffre.h"
#include "affiche_triangle.h"
#include "lire_args.h"
#include "length.h"


static char *sortie;

static char str_taille_police [MAX_INT_LENGTH];
static char str_x[MAX_INT_LENGTH];
static char str_y[MAX_INT_LENGTH];
static char str_delta_x[MAX_INT_LENGTH];
static char str_delta_y[MAX_INT_LENGTH];
static char str_valeur[MAX_INT_LENGTH];
static char str_nb_car[MAX_INT_LENGTH];

static char * liste [] = {"./creer_boite", str_taille_police, 
			    NULL, str_x, str_y, 
			    str_delta_x, str_delta_y, 
			    str_valeur, str_nb_car,NULL};


static char message_usage [] = "nombre d'arguments incorrect\nUsage : triangle taille_police nb_lignes";

/****************************************/
/*	Arguments pour gv question2			*/
/****************************************/

char nomGV [] = "/usr/bin/gv";
char *argumentsGV [] = {"/usr/bin/gv", "triangle.ps", NULL};

/***************************************/
/*  Arguments pour gs question 2       */
/**************************************/

char nomGS [] = "/usr/bin/gs";
char *argumentsGS [] = {"/usr/bin/gs", "-q", "-sDEVICE=x11", "triangle.ps", NULL};

/*********************************************************************/
/* Pour tracer les appels de boite_chiffre                          */
/*********************************************************************/

void trace_boite_chiffre (unsigned int taille_police, char *sortie,
		          unsigned int x, unsigned int y,
		          unsigned int delta_x, unsigned int delta_y,
		          unsigned int valeur, unsigned int nb_car)
{
  sprintf (str_taille_police,"%d",taille_police);
  liste[2] = sortie;
  sprintf (str_x,"%d",x); sprintf (str_y,"%d",y);
  sprintf (str_delta_y,"%d",delta_y); sprintf (str_delta_x,"%d",delta_x);
  sprintf (str_valeur,"%d",valeur); sprintf (str_nb_car,"%d",nb_car);
  creer_boite(9,liste,0);
}


/*********************************************************************/
/* La double boucle de parcours du triangle de Pascal                */
/* Tableau trait� comme un tableau � une seule dimension             */
/* pour que l'on puisse �ventuellement creer le tableau par malloc   */
/*********************************************************************/

void postscript_triangle (unsigned int taille_police)
{
  unsigned int nombre_lignes, ligne, colonne, nb_car;
  unsigned int x, y, valeur, delta_x, delta_y;
  char tampon [12];

  nombre_lignes=taille_triangle;
  triangle_pascal ();
  sprintf (tampon,"%d",maximum_pascal);
  nb_car = strlen (tampon);

  delta_x = (nb_car +1) * RATIO_DELTA_X * taille_police ;
  delta_y = RATIO_DELTA_Y * taille_police;

  for (ligne=0; ligne< nombre_lignes; ligne++)
    for (colonne=0; colonne<= ligne; colonne++)
      {
      valeur =pascal(ligne,colonne);
      if (valeur != 0)
        {
        y = ORIGINE_Y + delta_y * (nombre_lignes - (ligne+1));
        x = ORIGINE_X + delta_x * (colonne + 1);
#ifdef TRACE_BOITE
        trace_boite_chiffre (taille_police, sortie, x,y, delta_x, delta_y, 
               valeur, nb_car);
#endif
        boite_chiffre (taille_police, sortie, x,y, delta_x, delta_y, 
               valeur, nb_car);
        }
      }
}

/*********************************************************************/
/* Le main qui appelle tout le reste ...                             */
/*********************************************************************/

int main(int argc, char *argv[], char *envp[])
{
  unsigned int taille_police, nb_lignes;
  char nom_executable[200];
  pid_t p;
  int retour;

  lire_args(argc,argv,3,message_usage, 
        "%s",nom_executable,"",
        "%d",&taille_police,"taille_de_police_incorrecte",
        "%d",&nb_lignes,"nombre de lignes incorrect");

  /* Ici il faudrait ajouter une verification des valeurs */
  /* de taille_police [8,24] et nb_lignes [1,MAX_LIGNES]  */
  if(taille_police < 8 || taille_police > 24){
	  fprintf(stderr, "Taille de police incorrecte, valeur entre 8 et 24 attendue\n");
	  exit(1);
  }
  if(nb_lignes < 1 || nb_lignes > MAX_LIGNES){
	  fprintf(stderr, "Nombre de lignes incorrecte, valeur entre 1 et %d attendue\n", MAX_LIGNES);
	  exit(1);
  }

	
	p = fork();
	
	/* Verification de la cr�ation d'un fils a l'aide de la commande fork */
	if(p < 0) {
		fprintf(stderr, "Luke (fils) n'a pas pu etre creer par fork\nVador son pere est triste\n");
		exit(1);
	}
	
	/* Fils creer  plus qu'a attendre qu'il finisse les triangles*/
	if(p != 0){
		/* Attendre la terminaison du fils */
		wait(&retour);
		fprintf(stderr, "Generation et affichage du triangle de Pascal termine\n");
	} else {	
		/* Le fils fait des triangles */
 		sortie = "stdout";
  		taille_triangle = nb_lignes;
  		postscript_triangle (taille_police);
  		sleep (3);
  		execve(nomGV, argumentsGV, envp);
 	 	//execve(nomGS, argumentsGS, envp);
 	}
	
  return 0;
}
