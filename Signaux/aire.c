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
int fd_terrain, nbVers;

/*
 * HANDLER
 */
void handlerAire(int sig, siginfo_t *info, void *context){

  static int vers[MAX_VERS];
  static ver_t tab[MAX_VERS];
  static int init = 0;
  int i, nbLigne, nbCol, no_err, nbVoisin, indLibre;
  coord_t *voisin = NULL;
  
  if(!init){
    for(i = 0; i < MAX_VERS; i++)
      vers[i] = -1;
    init = 1;
    nbVers = 0;
  }
  
  for(i = 0; i < MAX_VERS && info->si_pid != vers[i]; i++);
  
  if(i == MAX_VERS){
    for(i = 0; i < MAX_VERS && vers[i] != -1; i++);
    if(i == MAX_VERS){
      fprintf(stderr, "Le tableau est plein");
      exit(-1);
    }
    else{
      vers[i] = info->si_pid;
      nbVers++;
    }
  
  if(no_err = terrain_dim_lire(fd_terrain, &nbLigne, &nbCol))
      return (no_err);

  if(no_err = jeu_ver_initialiser( fd_terrain, nbLigne, nbCol, &tab[i]))
      return (no_err);

  terrain_marque_ecrire(fd_terrain, tab[i].tete, i + 'A');

  }

  else {
    if(( no_err = terrain_voisins_rechercher(fd_terrain, tab[i].tete, &voisin, &nbVoisin)))
      return (no_err);

    if(!(no_err = terrain_case_libre_rechercher( fd_terrain, voisin, nbVoisin, &indLibre)) && indLibre != -1 ){

      if((no_err = terrain_marque_ecrire(fd_terrain, voisin[indLibre], i + 'A')))
        return (no_err);
      tab[i].tete = voisin[indLibre];

      if(no_err = terrain_afficher(fd_terrain))
        return (no_err);

    }
    else {
      kill(info->si_pid, SIGUSR2);
      nbVers --;
    }
    if(nbVers == 0){
      if(no_err = terrain_afficher(fd_terrain))
        return (no_err);
      fprintf(stderr,"Ver gagnant : %c !\n", i+'A');
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
  
  /*----------*/

  printf("\n\t----- %s : Debut du jeu -----\n\n" , Nom_Prog );
     
  /***********
   * A FAIRE * 
   ***********/
  fd_terrain = open(fich_terrain, O_RDWR);

  struct sigaction action;
  action.sa_sigaction = handlerAire;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1, &action, NULL);

  do{
    pause();
  }while(nbVers);
  
  printf("\n\n\t----- %s : Fin du jeu -----\n\n" , Nom_Prog );
  
  exit(0);
}


