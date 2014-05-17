#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
int main(int argc, char **argv)
{

	int n = atoi(argv[1]) ; // read argument from command line
	int superdaddy = getpid() ;
	int p[2],pipe0[2] ; // pipe
	int output, input, result1, result, cpid, savedinput;
	pipe(pipe0);
	output = STDOUT_FILENO ;

	result = 0;
	input = pipe0[0];
	if ( write(pipe0[1],&result,sizeof(result)) < 0 ) {
		// the first pipe for 0 
		perror(NULL) ;
		return 1 ;
	}
	if (close(pipe0[1])<0)
	{ // close unused pipe ends
		perror(NULL) ;
		return 1 ;
	}


	for (int i = 1; i <= n; i++)
	{ // every new child must go through the while body again
		if ( pipe(p) < 0 )
		{ // build pipe to return result
			perror(NULL) ;
			return 1 ;
		}
		savedinput = input;
		input = p[0];
		output = p[1]; // useful when parent spawns 2 children (pipe will now be with current and its child)
		cpid = fork() ; // create child i
		if ( cpid == 0 )// child
		{
			//process p-1 => process p
			if (read(savedinput,&result1,sizeof(result1)) < 0 )  //get the result ifrom previous
			{
				perror(NULL) ;
				return 1 ;
			}
			
			result = result1 + 1;

			if ( write(output,&result,sizeof(result)) < 0 ) { //write to current pipe end
				// each process will send the computed partial result to its own children
				perror(NULL) ;
				return 1 ;
			}

			if (close(output)<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
			if (close(input)<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}

			return 0;
		}else if (cpid<0)
		{
			perror(NULL) ;
			return 1 ;
		}
	}
	if (read(input,&result,sizeof(result)) < 0 ) 
	{
		perror(NULL) ;
		return 1 ;
	}

	printf("Result: %d\n", result) ;
	if (close(input)<0)
	{ // close unused pipe ends
		perror(NULL) ;
		return 1 ;
	}


	wait(NULL) ; // wait for the child to terminate (avoid zombies)
	return 0 ;
}