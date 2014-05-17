	#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#define STACKSIZE 10000
#define NUMPROCS 5
#define ROUNDS 10
sem_t s[NUMPROCS] ;
int pid[NUMPROCS];

int child () {
	int current_process_id = getpid();
	int i;
	int pos;
	
	for (i = 0; i < NUMPROCS; i++) 
		if (pid[i] == current_process_id ) {
			pos = i;
			break;
		}

	for (i = 0; i < ROUNDS ; i ++ ) {
		if (pos == NUMPROCS - 1) {
			sem_wait(&s[(pos + 1) % NUMPROCS]);    
			sem_wait(&s[pos]);                     
		} else {
			sem_wait(&s[pos]);                     
			sem_wait(&s[(pos + 1) % NUMPROCS]);    
		}
	
		int sleeptime = rand()%20000;
		if ( sleeptime > 10000 ) usleep(sleeptime) ;
	
		if (pos == NUMPROCS - 1) {
			sem_post(&s[pos]);                     
			sem_post(&s[(pos + 1) % NUMPROCS]);    
		} else {
			sem_post(&s[(pos + 1) % NUMPROCS]);    
			sem_post(&s[pos]);            
		}
	}
	
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i ;
	void * p ;
	srand(time(NULL));
	for ( i = 0 ; i < NUMPROCS ; i ++ ) sem_init(&s[i], 1, 1);
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
	for ( ; i > 0 ; i -- ) wait(NULL) ;
	return 0 ;
}	
