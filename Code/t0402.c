#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
int n = 1 ;
int childpid ;
childpid = fork() ;
printf("In process %d, n=%d\n", getpid(), n) ;
if ( childpid > 0 ) {
n = 10 ;
sleep(1) ;
} else {
execl("./t0402","t0402",NULL) ;
}
printf("In process %d, n=%d\n", getpid(), n) ;
if ( childpid > 0 ) wait(NULL) ;
return 0 ;
}