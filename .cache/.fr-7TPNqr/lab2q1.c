#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#define   BUF_SIZE   100

int main(int argv, char* argc[])
{
     char   buf[BUF_SIZE];
     int i;
    pid_t wpid;
    int status = 0;
     int n = atoi(argc[1]);
	for (i  = 1; i <= atoi(argc[1]); i++)
	{
		pid_t pid = fork();
		if (pid == 0)
          	{
			printf("Load %d\n",i);
			execl("/usr/bin/xterm", "xterm", NULL);
		}
	}

	while (wait(&status) > 0)
	{
	}
	exit(0);	

}
