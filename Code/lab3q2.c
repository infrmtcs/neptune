#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
int main(int argc, char **argv)
{

	int n = atoi(argv[1]) ; // read argument from command line
	int superdaddy = getpid() ;
	int p[2],pipe0[2] ; // pipe , pipe0 is the separated pipe for the last stage
	int output, input, result1, result, cpid;
	pipe(pipe0);
	output = STDOUT_FILENO ;
	while ( 1 )
	{ // every new child must go through the while body again
		n -- ; // argument for child

		if ( getpid()!=superdaddy && read(input,&result1,sizeof(result1)) < 0 ) 
		{
			perror(NULL) ;
			return 1 ;
		}
		result = result1 + 1;
		if ( n == -1 )
		{ // base case
			if ( write(pipe0[1],&result,sizeof(result)) < 0 ) {
				// at the last stage, send from process n - 1 to process 0
				perror(NULL) ;
				return 1 ;
			}
			if (close(pipe0[1])<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
			return 0;
		}

		if ( pipe(p) < 0 )
		{ // build pipe to return result
			perror(NULL) ;
			return 1 ;
		}
		//printf("%d\n",n);
		input = p[0];
		output = p[1]; // useful when parent spawns 2 children (pipe will now be with current and its child)
		cpid = fork() ; // create child 
		if ( cpid == 0 )// child
		{
			if (close(p[1])<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
			continue ;
		}else if (cpid<0)
		{
			perror(NULL) ;
			return 1 ;
		}
		if (close(p[0])<0)
		{ // close unused pipe ends
			perror(NULL) ;
			return 1 ;
		}
		break;
	}
	if ( write(output,&result,sizeof(result)) < 0 ) {
		// each process will send the computed partial result to its own children
		perror(NULL) ;
		return 1 ;
	}
	if ( getpid()==superdaddy ) // the original parent prints the result
		{
			result = 0;
			if ( write(output,&result,sizeof(result)) < 0 ) {
				//process 0 first send 0 to process 1
				perror(NULL) ;
				return 1 ;
			}
			if (close(output)<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
			read(pipe0[0],&result,sizeof(result));
			//process 0 retrieve the result from processn
			printf("Result: %d\n", result - 1) ;
			if (close(pipe0[0])<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
		}
	wait(NULL) ; // wait for the child to terminate (avoid zombies)
	return 0 ;
}