#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
int quit=0;
void sigHandler(int signal)
{
if(signal==SIGTERM)
{
printf("Goodbye!\n");
quit=1;
}
}
int main()
{
FILE *fp;
struct sigaction action;
memset(&action, 0, sizeof(struct sigaction));
action.sa_handler=sigHandler;
sigaction(SIGTERM, &action, NULL);
if ((fp==fopen("test.txt", "rt"))!=NULL)
{
while(!feof(fp) && !quit)
{
int data;
fscanf(fp, "%d", &data);
} // while
fclose(fp);
} // if
}