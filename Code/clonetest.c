#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#define STACKSIZE 10000
#define NUMPROCS 10
#define ROUNDS 10
sem_t s[NUMPROCS] ;
int pid[NUMPROCS];
int q;

int child () {
	int cur = getpid();
	int i;
	//find current process index
	q++;
	i = q;
	int z = 0;
	for (z = 0; z < ROUNDS; z++)
	printf("%d",i);
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
		// create a child that shares the data segment
		// the stack must be at the top of the allocated area
		int c = clone(child,p + STACKSIZE - 1,CLONE_VM|SIGCHLD,NULL) ;
		//printf("created %d",c);
		pid[i] = c;
		if ( c < 0 ) {
			perror(NULL) ;
			exit(1) ;
		}
	}
	for ( ; i > 0 ; i -- ) wait(NULL) ;
	return 0 ;
}	