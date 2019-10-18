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

  if((fd_terrain = open(fich_terrain, O_APPEND)) == -1){
    fprintf(stderr,"Erreur lors de l'ouverture du ficher\n");
  }

  stat(fich_terrain, &infosTerrain);
  date_prec = infosTerrain.st_mtime;

  while(1){

    if(date_prec < infosTerrain.st_mtime){

      /* Pose du verrou */
      verrou_terrain.l_type = F_RDLCK;
      verrou_terrain.l_start = 0;
      verrou_terrain.l_len = 0;
      verrou_terrain.l_whence = 0;
      fcntl(fd_terrain, F_SETLKW, &verrou_terrain);

      /* Affichage du terrain */
      if(no_err = terrain_afficher(fd_terrain)){
        fprintf(stderr,"Erreur lors de l'affichage du terrain\n");
        exit(no_err);
      }
      
      /* Retrait du verrou */
      verrou_terrain.l_type = F_UNLCK;
      fcntl(fd_terrain, F_SETLKW, &verrou_terrain);
      date_prec = infosTerrain.st_mtime;

    }
    stat(fich_terrain, &infosTerrain);
    sleep(1);
  }
  close(fd_terrain);
  printf("\n%s : --- Arret de l'affichage du terrain ---\n", nomprog );

  exit(0);
}

