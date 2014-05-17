#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char ** argv) {
	int n = atoi(argv[1]) ;
	int pid = getpid() ;
	int childpid = 0 ;
	printf("Superdaddy: %d\n", pid) ;
	int c = 1;
	while ( c == 1 ) {
		if (n==0) break;
		childpid = fork() ;
		if (childpid == 0) {
			n--;
			continue;
		} else if (childpid > 0) {
		} else perror(strerror(errno)) ;

		childpid = fork() ;
		if (childpid == 0) {
			n--;
			continue;
		} else if (childpid > 0) {
			break;
		} else perror(strerror(errno)) ;

	}
	sleep(30);
	return 0 ;
}

