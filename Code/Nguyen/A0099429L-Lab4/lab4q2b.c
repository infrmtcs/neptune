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
#define ROUNDS 1000
#define TIMESEED 10000
int farmer_in = 0, cabbage_in = 0, goat_in = 0, wolf_in = 0 ;
int cabbage_goat[2], goat_wolf[2],v;
int farmer() {
	int i ;
	for ( i = 0 ; i < ROUNDS ; i ++ ) {
		farmer_in = 1 ; // Farmer notifies the world that he is in
		if ( write(goat_wolf[1],&v,sizeof(v)) < 0 ) { // release chopstick
			perror(NULL) ;
			return 1 ;
		}
		if ( write(cabbage_goat[1],&v,sizeof(v)) < 0 ) { // release chopstick
			perror(NULL) ;
			return 1 ;
		}
		// Entry code here (to allow other processes to go in)
		printf("Farmer is in\n") ;
		usleep(rand()%TIMESEED) ; // Stay in for a little while
		
		if ( read(cabbage_goat[0],&v,sizeof(v)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		if ( read(goat_wolf[0],&v,sizeof(v)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		// Exit code here (may need to wait; immediate exit may lead to dangerous conditions)
		farmer_in = 0 ; // Farmer notifies the world that he is out
		printf("Farmer is out\n") ;
		usleep(rand()%TIMESEED) ;
	}
	return 0 ;
}
int cabbage() {
	int i ;
	for ( i = 0 ; i < ROUNDS ; i ++ ) {
		if ( read(cabbage_goat[0],&v,sizeof(v)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		// Entry code here (wait to avoid dangerous conditions)
		cabbage_in = 1 ; // Cabbage notifies the world that it went in
		printf("Cabbage is in\n") ;
		// Detect dangerous conditions
		if ( goat_in && ! farmer_in )
		// This should never execute
		printf("\"HELP, I'M GONNA BE EATEN!\" the Cabbage says.\n");
		usleep(rand()%TIMESEED) ; // stay in for a little while
		cabbage_in = 0 ; // Cabbage notifies the world that it went out
		if ( write(cabbage_goat[1],&v,sizeof(v)) < 0 ) // take chopstick
		{
			perror(NULL) ;
			return 1 ;
		}
		// Exit code here (to allow other processes to go in)
		printf("Cabbage is out\n") ;
		usleep(rand()%TIMESEED) ; // stay out for a little out
	}
	return 0 ;
}
int goat() {
	int i ;
	for ( i = 0 ; i < ROUNDS ; i ++ ) {
		if ( read(cabbage_goat[0],&v,sizeof(v)) < 0 ) 
		{
			perror(NULL) ;
			return 1 ;
		}
		if ( read(goat_wolf[0],&v,sizeof(v)) < 0 )
		{
			perror(NULL) ;
			return 1 ;
		}
		// Entry code here (to avoid dangerous conditions)
		goat_in = 1 ; // Notify the world that goat is in
		printf("Goat is in\n") ;
		// Detect dangerous conditions
		if ( cabbage_in && ! farmer_in )
		// This should never execute
		printf("\"THE CABBAGE LOOKS SO YUMMY!\" the Goat says.\n");
		if ( wolf_in && ! farmer_in )
		// This should never execute
		printf("\"HELP, I'M GONNA BE EATEN!\" the Goat says.\n");
		usleep(rand()%TIMESEED) ; // stay in for a little while
		goat_in = 0 ; // Notify the world that goat is out
		if ( write(goat_wolf[1],&v,sizeof(v)) < 0 ) { 
			perror(NULL) ;
			return 1 ;
		}
		if ( write(cabbage_goat[1],&v,sizeof(v)) < 0 ) { 
			perror(NULL) ;
			return 1 ;
		}
		// Exit code here (to let other processes go in)
		printf("Goat is out\n") ;
		usleep(rand()%TIMESEED) ; // stay out for a little while
	}
	return 0 ;
}
int wolf() {
	int i ;
	for ( i = 0 ; i < ROUNDS ; i ++ ) {
		if ( read(goat_wolf[0],&v,sizeof(v)) < 0 ) 
		{
			perror(NULL) ;
			return 1 ;
		}
		// Entry code here (wait to avoid dangerous conditions)
		wolf_in = 1 ; // Notify the world that wolf is in
		printf("Wolf is in\n") ;
		// Detect dangerous conditions
		if ( goat_in && ! farmer_in )
		// This should never execute
		printf("\"THE GOAT LOOKS SO YUMMY!\" the Wolf says.\n");
		usleep(rand()%TIMESEED) ; // stay in for a little while
		wolf_in = 0 ; // Notify the world that wolf is out
		if ( write(goat_wolf[1],&v,sizeof(v)) < 0 ) { 
			perror(NULL) ;
			return 1 ;
		}
		// Exit code here (to let other processes go in)
		printf("Wolf is out\n") ;
		usleep(rand()%TIMESEED) ; // Stay out for a little while
	}
	return 0 ;
}
int main ( int argc, char ** argv ) {
	int i, (*child[4])() ;
	void * stack ;
	if ( pipe(cabbage_goat) < 0 )
	{ 
		perror(NULL) ;
		return 1 ;
	}
	if ( write(cabbage_goat[1],&v,sizeof(v)) < 0 ) {
		perror(NULL) ;
		return 1 ;
	}
	if ( pipe(goat_wolf) < 0 )
	{ 
		perror(NULL) ;
		return 1 ;
	}
	if ( write(goat_wolf[1],&v,sizeof(v)) < 0 ) {
		perror(NULL) ;
		return 1 ;
	}

	// initialize the array of processes
	child[0] = farmer ; child[1] = cabbage ; child[2] = goat ; child[3] = wolf ;
	srand(time(NULL)); // initialize the random seed
	// start all processes in the array
	for ( i = 0 ; i < 4 ; i ++ ) {if ( (stack = malloc(STACKSIZE)) == NULL ) {
			printf("Memory allocation error\n") ;
			exit(1) ;
		}
		int c = clone(child[i],stack+STACKSIZE-1,CLONE_VM|SIGCHLD,NULL) ;
		if ( c < 0 ) {
			perror(NULL) ;
			exit(1) ;
		}
	}
	if (close(cabbage_goat[0])<0)
	{ 
		perror(NULL) ;
		return 1 ;
	}
	if (close(cabbage_goat[1])<0)
	{ 
		perror(NULL) ;
		return 1 ;
	}
	if (close(goat_wolf[0])<0)
	{ 
		perror(NULL) ;
		return 1 ;
	}
	if (close(goat_wolf[1])<0)
	{ 
		perror(NULL) ;
		return 1 ;
	}
	// Wait for all children to terminate
	for ( i = 0 ; i < 4 ; i ++ ) {
	wait(NULL) ;
	}
	return 0 ;
}
