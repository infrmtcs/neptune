#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int i;
#define BUFF_SIZE 8
char s[BUFF_SIZE+1];
char s2[BUFF_SIZE+1];
int l,l2;

int main(int argc,char *argv[]){
    int in_fd1 = open(argv[1], O_RDONLY);
    int in_fd2 = open(argv[2], O_RDONLY);
    if (in_fd1 < 0 || in_fd2 < 0)
    {
        if (errno == EEXIST)
        {
            printf("file not exist");
            return 0;
        }
    }

    memset(s,0,sizeof(s));
    while ((l = read(in_fd1,s,BUFF_SIZE)) > 0){
        memset(s2,0,sizeof(s2));
        l2 = read(in_fd2,s2,BUFF_SIZE);
        if (l != l2)
        {
            printf("Two files are different");
            close(in_fd1);
            close(in_fd2);
            return 0;
        }
        if (strcmp(s,s2)!=0)
        {
            printf("Two files are different");
            close(in_fd1);
            close(in_fd2);
            return 0;
        }
        memset(s,0,sizeof(s));
    }
    printf("Two files are identical");
    close(in_fd1);
    close(in_fd2);
    return 0;
}

