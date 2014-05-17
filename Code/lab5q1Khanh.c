#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char** argv){
    if (argc<4){
        printf("Not enough parameters (need 3)\n");
        return 1;
    }
    
    char* fname = (char*)argv[1];
    char* str1 = (char*)argv[2];
    int offset = atoi(argv[3]);
    
    int file1 = open(fname, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    int len = lseek(file1, 0, SEEK_END);
    
    printf("%d\n", len);
    
    if (offset>len) {
        char s[] = {' ', 0};
        int i;
        for (i=len+1; i<=offset; i++) write(file1, s, 1);
    }
    
    lseek(file1, offset, SEEK_SET);
    write(file1, str1, strlen(str1));    
    
    close(file1);
    return 0;
}
