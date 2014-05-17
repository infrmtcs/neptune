// Compute the factorial of a number by spawning new
// processes instead of performing recursive calls.
// Each child sends its partial result to the parent
// via a pipe.
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
	int output, input, result1, result, cpid, parentinput;
	pipe(pipe0);
	output = STDOUT_FILENO ;
	while ( 1 )
	{ // every new child must go through the while body again
		if ( n == 0 )
		{ // base case
			/*if ( write(output,&n,sizeof(n)) < 0 ) {
				perror(NULL) ;
				return 1 ;
			}
			return 0 ;*/
			break;
		}
		n -- ; // argument for child
		if ( pipe(p) < 0 )
		{ // build pipe to return result
			perror(NULL) ;
			return 1 ;
		}
		//printf("%d\n",n);
		parentinput = input;
		input = p[0];
		output = p[1]; // useful when parent spawns 2 children (pipe will now be with current and its child)
		cpid = fork() ; // create child to compute factorial(n-1)
		if ( cpid == 0 )// child
		{
			if (close(p[0])<0)
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
/*		if (close(p[0])<0)
		{ // close unused pipe ends
			perror(NULL) ;
			return 1 ;
		}
		if (close(p[1])<0)
		{ // close unused pipe ends
			perror(NULL) ;
			return 1 ;
		}
*/
		break ;
	}
	if ( getpid()==superdaddy ) // the original parent prints the result
		{
			result = 0;
			if ( write(output,&result,sizeof(result)) < 0 ) {
				// each process will send the computed partial result to its own children
				perror(NULL) ;
				return 1 ;
			}
			printf("%d %d\n",n,result);
			read(pipe0[0],&result,sizeof(result));
			printf("Result: %d\n", result) ;
			if (close(pipe0[0])<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
		}
	else if (n == 0)
		{
			if ( read(parentinput,&result1,sizeof(result1)) < 0 ) // co close ko nhi
			{
				perror(NULL) ;
				return 1 ;
			}
			result = result1 + 1 ; // res = res + 1
			printf("ACE %d %d\n",n,result);
			if ( write(pipe0[1],&result,sizeof(result)) < 0 ) {
				// each process will send the computed partial result to its own children
				perror(NULL) ;
				return 1 ;
			}
			if (close(pipe0[1])<0)
			{ // close unused pipe ends
				perror(NULL) ;
				return 1 ;
			}
		}
	else 
	{
		if ( read(parentinput,&result1,sizeof(result1)) < 0 ) // co close ko nhi
		{
			perror(NULL) ;
			return 1 ;
		}
		result = result1 + 1 ; // res = res + 1
		printf("%d %d\n",n,result);
		if ( write(output,&result,sizeof(result)) < 0 ) {
			// each process will send the computed partial result to its own children
			perror(NULL) ;
			return 1 ;
		}
	}

	wait(NULL) ; // wait for the child to terminate (avoid zombies)
	return 0 ;
}