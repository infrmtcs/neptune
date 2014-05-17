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
int temp;

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
	if (idx == NUMPROCS - 1)  // pick right and then left for Nth person to avoid deadlock case when everyone takes the chopstick in same direction
	{
		for (i = 0; i < ROUNDS ; i ++ ) {
			if ( read(s[(idx + 1) % NUMPROCS][0],&temp,sizeof(temp)) < 0 ) // take chopstick
			{
				perror(NULL) ;
				return 1 ;
			}
			if ( read(s[idx][0],&temp,sizeof(temp)) < 0 ) // take chopstick
			{
				perror(NULL) ;
				return 1 ;
			}

			// Add your entry protocol here
			// Start of critical section -- simulation of slow n++
			printf("philosopher %d picks chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
			int sleeptime = rand()%20000 ;
			if ( sleeptime > 10000 ) usleep(sleeptime) ;
			// End of critical section

			if ( write(s[idx][1],&temp,sizeof(temp)) < 0 ) { // release chopstick
				perror(NULL) ;
				return 1 ;
			}
			if ( write(s[(idx + 1) % NUMPROCS][1],&temp,sizeof(temp)) < 0 ) { // release chopstick
				perror(NULL) ;
				return 1 ;
			}

			printf("philosopher %d releases chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
			// Add your exit protocol here
		}

	}
	else // pick left and then right
	for (i = 0; i < ROUNDS ; i ++ ) {
		if ( read(s[idx][0],&temp,sizeof(temp)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		if ( read(s[(idx + 1) % NUMPROCS][0],&temp,sizeof(temp)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		// Add your entry protocol here
		// Start of critical section -- simulation of slow n++
		printf("philosopher %d picks chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
		int sleeptime = rand()%20000 ;
		if ( sleeptime > 10000 ) usleep(sleeptime) ;
		// End of critical section
		if ( write(s[(idx + 1) % NUMPROCS][1],&temp,sizeof(temp)) < 0 ) { // release chopstick
			perror(NULL) ;
			return 1 ;
		}
		if ( write(s[idx][1],&temp,sizeof(temp)) < 0 ) { // release chopstick
			perror(NULL) ;
			return 1 ;
		}
		printf("philosopher %d releases chopstick %d and %d\n",idx,idx,(idx + 1) % NUMPROCS);
		// Add your exit protocol here
	}
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i ;
	void * p ;
	srand(time(NULL));
	for ( i = 0 ; i < NUMPROCS ; i ++ ) 
		{
			//semaphore init
			if ( pipe(s[i]) < 0 )
			{ 
				perror(NULL) ;
				return 1 ;
			}

			//init semaphore
			if ( write(s[i][1],&temp,sizeof(temp)) < 0 ) {
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

	for ( i = 0; i < NUMPROCS ; i ++ ) 
	{
		if (close(s[i][0])<0)
		{ // close unused pipe ends
			perror(NULL) ;
			return 1 ;
		}
		if (close(s[i][1])<0)
		{ // close unused pipe ends
			perror(NULL) ;
			return 1 ;
		}

	}
	for ( ; i > 0 ; i -- ) wait(NULL) ;
	return 0 ;
}	