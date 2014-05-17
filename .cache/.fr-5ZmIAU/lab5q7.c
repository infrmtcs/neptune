#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>



int main(int argc,char *argv[]){
    struct stat s1;
    struct stat s2;

    if (stat(argv[1],&s1) < 0) printf("cannot find the file %s",argv[1]);
    if (stat(argv[2],&s2) < 0) printf("cannot find the file %s",argv[2]);

    if (s1.st_ino == s2.st_ino)
        printf("Two files names represent hard links to the same file");
    else
        printf("Two files names don't represent hard links to the same file");
    
    return 0;
}
