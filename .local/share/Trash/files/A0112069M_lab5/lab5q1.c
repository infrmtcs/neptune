#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int i;

int main(int argc,char *argv[]){
    int output_fd = open(argv[1], O_CREAT|O_WRONLY,S_IRWXU | S_IRWXG | S_IRWXO);
    if (output_fd < 0)
    {
        if (errno == EEXIST)
        printf("not exist and create file");
    }
    int pos = atoi(argv[3]);
    int filelen = lseek(output_fd, 0, SEEK_END);
    if (pos>filelen) {
        int i;
        for (i=filelen+1; i<=pos; i++) write(output_fd, " ", 1);
    }

    lseek(output_fd,pos, SEEK_SET);
    write(output_fd,argv[2],strlen(argv[2]));
    close(output_fd);
    return 0;
}
