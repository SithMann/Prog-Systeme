#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <commun.h>
#include <terrain.h>
#include <vers.h>
#include <jeu.h>
#include <signaux.h>

/* 
 * VARIABLES GLOBALES (utilisees dans les handlers)
 */

#define MAX_VERS 26
char Nom_Prog[256] ;
int fd;



/*
 * HANDLER
 */
void handler_aire(int sig, siginfo_t *info, void *context){
  static int tabVers[MAX_VERS];//On veut qu'il garde en mémoire les vers présents
  static ver_t tab[MAX_VERS];
  static int init = 0;// Toujours pour ne pas le changer à chaque appel de la fonction handler;
  int i, j, nbLigne, nbCol, no_err, nbVoisin, indLibre;
  coord_t *voisin = NULL;
  
  if(init == 0){
    for(i = 0; tabVers[i]; i++)
      tabVers[i] = -1;
    init = 1;
  }
  
  for(i = 0; i < MAX_VERS && info->si_pid != tabVers[i]; i++);//i est une marque
  
  if(i == MAX_VERS){
    for(i = 0; i < MAX_VERS && tabVers[i] != -1; i++);//si le tableau est plein ou si on a trouvé un emplacement libre
    if(i == MAX_VERS){
      fprintf(stderr, "Le tableau est plein");
      //kill(, SIGUSR2);
      exit(-1);
    }
    else
      tabVers[i] = info->si_pid;
  
  if ( terrain_dim_lire(fd, &nbLigne, &nbCol) )
      return -1;

  if ( jeu_ver_initialiser( fd, nbLigne, nbCol, &tab[i]) )
      return -2;

  terrain_marque_ecrire( fd, tab[i].tete, i + 'A');//i + 'A' a utiliser dans marque ecrire

  }

  else {
    if ( ( no_err = terrain_voisins_rechercher( fd, tab[i].tete, &voisin, &nbVoisin )) )
      return -7;

    if ( !( no_err = terrain_case_libre_rechercher( fd, voisin, nbVoisin, &indLibre ) ) && indLibre != -1 ){

      if( (no_err = terrain_marque_ecrire( fd, voisin[indLibre], i + 'A') ) )
        return -8;

      if(no_err = terrain_afficher(fd))
        exit(-9);

    }
    else {
      kill(info->si_pid, SIGUSR2);
    }
  }
  
}


int main( int nb_arg , char * tab_arg[] ){
  char fich_terrain[128] ;
  pid_t pid_aire ; 

  /*----------*/
  
  /* 
   * Capture des parametres 
   */
  strcpy( Nom_Prog , tab_arg[0] );

  if( nb_arg != 2 ){
    fprintf( stderr , "Usage : %s <fichier terrain>\n", Nom_Prog );
    exit(-1);
  }

  strcpy( fich_terrain , tab_arg[1] );
  pid_aire = getpid() ; 

  /* Affichage du pid pour les processus verDeTerre */
  printf( "\n\t-----------------.\n") ; 
  printf( "--- pid %s = %d ---\n" , Nom_Prog , pid_aire ) ; 
  printf(   "\t-----------------.\n\n") ; 
  
  /* Initialisation de la generation des nombres pseudo-aleatoires */
  srandom((unsigned int)pid_aire);
  fd = open(fich_terrain, O_RDWR);
  /*----------*/

  printf("\n\t----- %s : Debut du jeu -----\n\n" , Nom_Prog );
     
  /***********
   * A FAIRE * 
   ***********/

  struct sigaction act;










  
  printf("\n\n\t----- %s : Fin du jeu -----\n\n" , Nom_Prog );
  
  exit(0);
}


