#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <errno.h>
#define STACKSIZE 10000
#define NUMPROCS 5
#define ROUNDS 10
int s[NUMPROCS][2] ;
int pid[NUMPROCS];
int v;

int child () {
	int current_process_id = getpid();
	int i;
	int pos;
	
	for (i = 0; i < NUMPROCS; i++) if (pid[i] ==  current_process_id) pos = i;

	for (i = 0; i < ROUNDS ; i ++ ) {
		if (pos == NUMPROCS - 1) {
			if ( read(s[(pos + 1) % NUMPROCS][0],&v,sizeof(v)) < 0 )
			{
				perror(NULL) ;
				return 1 ;
			}
			if ( read(s[pos][0],&v,sizeof(v)) < 0 ) 
			{
				perror(NULL) ;
				return 1 ;
			}
		} else {
			if ( read(s[pos][0],&v,sizeof(v)) < 0 ) 
			{
				perror(NULL) ;
				return 1 ;
			}
			if ( read(s[(pos + 1) % NUMPROCS][0],&v,sizeof(v)) < 0 ) 
			{
				perror(NULL) ;
				return 1 ;
			}
		}

		int sleeptime = rand()%20000 ;
		if ( sleeptime > 10000 ) usleep(sleeptime) ;
		
		if (pos == NUMPROCS - 1) {
			if ( write(s[pos][1],&v,sizeof(v)) < 0 ) { 
				perror(NULL) ;
				return 1 ;
			}
			if ( write(s[(pos + 1) % NUMPROCS][1],&v,sizeof(v)) < 0 ) { 
				perror(NULL) ;
				return 1 ;
			}
		} else {
			if ( write(s[(pos + 1) % NUMPROCS][1],&v,sizeof(v)) < 0 ) { 
				perror(NULL) ;
				return 1 ;
			}
			if ( write(s[pos][1],&v,sizeof(v)) < 0 ) { 
				perror(NULL) ;
				return 1 ;
			}
		}
	}
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i ;
	void * p ;
	srand(time(NULL));
	for ( i = 0 ; i < NUMPROCS ; i ++ ) 
		{

			if ( pipe(s[i]) < 0 )
			{ 
				perror(NULL) ;
				return 1 ;
			}


			if ( write(s[i][1],&v,sizeof(v)) < 0 ) {
				perror(NULL) ;
				return 1 ;
			}
		}

	for ( i = 0 ; i < NUMPROCS ; i ++ ) {
		p = malloc(STACKSIZE) ;
		if ( p == NULL ) {
			printf("Error allocating memory\n") ;
			exit(1) ;
		}
		
		int c = clone(child,p + STACKSIZE - 1,CLONE_VM|SIGCHLD,NULL) ;

		pid[i] = c;
		if ( c < 0 ) {
			perror(NULL) ;
			exit(1) ;
		}
	}

	for ( i = 0; i < NUMPROCS ; i ++ ) 
	{
		if (close(s[i][0])<0)
		{ 
			perror(NULL) ;
			return 1 ;
		}
		if (close(s[i][1])<0)
		{ 
			perror(NULL) ;
			return 1 ;
		}

	}
	for ( ; i > 0 ; i -- ) wait(NULL) ;
	return 0 ;
}	
