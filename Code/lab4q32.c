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
#define ROUNDS 100
sem_t ss[NUMPROCS];
int pid[NUMPROCS];
sem_t mutex;

int child () {
	int cur = getpid();
	int i;
	int idx;
	//find current process index
	for (i = 0; i < NUMPROCS; i++) 
		if (pid[i] == cur) {
			idx = i;
			break;
		}
	for (i = 0; i < ROUNDS ; i ++ ) {
		sem_wait(&ss[idx]);
		// Add your entry protocol here
		// Start of critical section -- simulation of slow n++
		printf("%d picks chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
		int sleeptime = rand()%20000 ;
		if ( sleeptime > 10000 ) usleep(sleeptime) ;
		// End of critical section
		
		sem_post(&ss[(idx + 1) % NUMPROCS]);
		sem_post(&ss[idx ]);

		sem_post(&ss[idx]);
		sem_post(&ss[(idx - 1 + NUMPROCS) % NUMPROCS]);
		
		printf("%d releases chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
		// Add your exit protocol here
	}
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i ;
	void * p ;
	srand(time(NULL));
	for ( i = 0 ; i < NUMPROCS ; i ++ ) {
		sem_init(&ss[i], 1, 2);
	}
	sem_init(&mutex,1,1);
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