#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

#include <commun.h>
#include <terrain.h>
#include <vers.h>
#include <jeu.h>

int verrou_voisins( int fd, int x, int y, int poser ) {
     off_t fdEmp;
    if ( poser ) {

        terrain_xy2pos( fd, x - 1, y - 1, &fdEmp);

        struct flock verrou1;
        verrou1.l_type = F_WRLCK;
        verrou1.l_start = fdEmp;
        verrou1.l_len = CASE_TAILLE * 3;
        verrou1.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou1);

        terrain_xy2pos( fd, x - 1, y, &fdEmp);

        struct flock verrou2;
        verrou2.l_type = F_WRLCK;
        verrou2.l_start = fdEmp;
        verrou2.l_len = CASE_TAILLE * 3;
        verrou2.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou2);

        terrain_xy2pos( fd, x - 1, y + 1, &fdEmp);

        struct flock verrou3;
        verrou3.l_type = F_WRLCK;
        verrou3.l_start = fdEmp;
        verrou3.l_len = CASE_TAILLE * 3;
        verrou3.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou3);

    } else {

        terrain_xy2pos( fd, x - 1, y - 1, &fdEmp);

        struct flock verrou1;
        verrou1.l_type = F_UNLCK;
        verrou1.l_start = fdEmp;
        verrou1.l_len = CASE_TAILLE * 3;
        verrou1.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou1);

        terrain_xy2pos( fd, x - 1, y, &fdEmp);

        struct flock verrou2;
        verrou2.l_type = F_UNLCK;
        verrou2.l_start = fdEmp;
        verrou2.l_len = CASE_TAILLE * 3;
        verrou2.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou2);

        terrain_xy2pos( fd, x - 1, y + 1, &fdEmp);

        struct flock verrou3;
        verrou3.l_type = F_UNLCK;
        verrou3.l_start = fdEmp;
        verrou3.l_len = CASE_TAILLE * 3;
        verrou3.l_whence = 0;
        fcntl(fd, F_SETLKW, &verrou3);
    }

    return 0;
}

int
main( int nb_arg , char * tab_arg[] )
{

     /* Parametres */
     char fich_terrain[128] ;
     case_t marque = CASE_LIBRE ;
     char nomprog[128] ;
     int no_err, fd;


     /*----------*/

     /* Capture des parametres */
     if( nb_arg != 3 )
     {
	  fprintf( stderr , "Usage : %s <fichier terrain> <marque>\n", 
		   tab_arg[0] );
	  exit(-1);
     }

     if( strlen(tab_arg[2]) !=1 ) 
     {
	  fprintf( stderr , "%s : erreur marque <%s> incorrecte \n",
		   tab_arg[0] , tab_arg[2] );
	  exit(-1) ;
     }

     strcpy( nomprog , tab_arg[0] );
     strcpy( fich_terrain , tab_arg[1] );
     marque = tab_arg[2][0] ;


   /* Initialisation de la generation des nombres pseudo-aleatoires */
     srandom((unsigned int)getpid());

     printf( "\n\n%s : ----- Debut du ver %c (%d) -----\n\n ", 
	     nomprog , marque , getpid() );

     /***********/
     /* A FAIRE */
     /***********/

    fd = open( fich_terrain, O_RDWR );

    struct flock verrou_terrain;
    verrou_terrain.l_type = F_RDLCK;
    verrou_terrain.l_start = 0;
    verrou_terrain.l_len = sizeof(int) * 2;
    verrou_terrain.l_whence = 0;
    fcntl(fd, F_SETLKW, &verrou_terrain);

    if (no_err = terrain_dim_lire(fd, &nbLigne, &nbCol) )
        return no_err;

    verrou_terrain.l_type = F_UNLCK;
    fcntl(fd, F_SETLKW, &verrou_terrain);

    if ( no_err = jeu_ver_initialiser( fd, nbLigne, nbCol, &ver) )
        return no_err;

    verrou_voisins( fd, ver.tete.x, ver.tete.y, 1 );
    
    if ( ( no_err = terrain_voisins_rechercher( fd, ver.tete, &voisin, &nbVoisin )) )
        return no_err;

    while ( !( no_err = terrain_case_libre_rechercher( fd, voisin, nbVoisin, &indLibre ) ) && indLibre != -1 ) {

        if( (no_err = terrain_marque_ecrire( fd, voisin[indLibre], marque ) ) )
            return no_err;

        verrou_voisins( fd, ver.tete.x, ver.tete.y, 0 );

        ver.tete = voisin[indLibre]; 

        sleep( random() % TEMPS_MOYEN + 1 );

        verrou_voisins( fd, ver.tete.x, ver.tete.y, 1 );
        
        if ( ( no_err = terrain_voisins_rechercher( fd, ver.tete, &voisin, &nbVoisin )) )
            return -15;
    }

    verrou_voisins( fd, ver.tete.x, ver.tete.y, 0 );

    close(fd);
     /* fin à faire */

     printf( "\n\n%s : ----- Fin du ver %c (%d) -----\n\n ", 
	     nomprog , marque , getpid() );

     exit(0);
}
