#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include <commun.h>
#include <terrain.h>


/*--------------------* 
 * Main demon 
 *--------------------*/
int
main( int nb_arg , char * tab_arg[] )
{
  /* Parametres */
  char fich_terrain[128] ;
  char nomprog[256] ;
  struct stat infosTerrain;
  time_t date_prec;
  int no_err = 0;
  int fd_terrain;
  struct flock verrou_terrain;
  
     /*----------*/

  /* Capture des parametres */
  if( nb_arg != 2 )
    {
      fprintf( stderr , "Usage : %s <fichier terrain>\n", 
	       tab_arg[0] );
      exit(-1);
    }

  strcpy( nomprog , tab_arg[0] );
  strcpy( fich_terrain , tab_arg[1] );


  printf("\n%s : ----- Debut de l'affichage du terrain ----- \n", nomprog );

  /***********/
  /* A FAIRE */
  /***********/

  // On veut afficher le terrain, seulement lorsque le fic a été modifié.
  // Récuperer les infos du fic et regarder date.
  if((fd_terrain = open(fich_terrain, O_CREAT | O_EXCL)) == -1){
    fprintf(stderr,"Erreur lors de l'ouverture du ficher\n");
  }
  stat(fich_terrain, &infosTerrain);
  date_prec = infosTerrain.st_mtime;
  fcntl(fd_terrain, F_SETLK, &verrou_terrain);
  while(1){
    if(date_prec == infosTerrain.st_mtime){
      if(no_err = terrain_afficher(fd_terrain)){
        fprintf(stderr,"Erreur lors de l'affichage du terrain\n");
        exit(no_err);
      }
      date_prec = infosTerrain.st_mtime;
    }
    stat(fich_terrain, &infosTerrain);
    sleep(200);
  }
  close(fd_terrain);
  printf("\n%s : --- Arret de l'affichage du terrain ---\n", nomprog );

  exit(0);
}

