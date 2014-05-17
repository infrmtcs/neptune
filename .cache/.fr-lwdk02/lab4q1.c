#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#define STACKSIZE 10000
#define NUMPROCS 20
#define ROUNDS 10
int n = 0 ;
sem_t s ;

int child () {
	int i = 0 ;
	for ( ; i < ROUNDS ; i ++ ) {
		sem_wait(&s);
		// Add your entry protocol here
		// Start of critical section -- simulation of slow n++
		int tmp = n ;
		int sleeptime = rand()%20000 ;
		if ( sleeptime > 10000 ) usleep(sleeptime) ;n = tmp + 1 ;
		// End of critical section
		sem_post(&s);
		// Add your exit protocol here
	}
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i ;
	void * p ;
	srand(time(NULL));
	sem_init(&s, 1, 1);
	for ( i = 0 ; i < NUMPROCS ; i ++ ) {
		p = malloc(STACKSIZE) ;
		if ( p == NULL ) {
			printf("Error allocating memory\n") ;
			exit(1) ;
		}
		// create a child that shares the data segment
		// the stack must be at the top of the allocated area
		int c = clone(child,p + STACKSIZE - 1,CLONE_VM|SIGCHLD,NULL) ;
		if ( c < 0 ) {
			perror(NULL) ;
			exit(1) ;
		}
	}
	for ( ; i > 0 ; i -- ) wait(NULL) ;
	printf("n=%d\n",n);
	return 0 ;
}