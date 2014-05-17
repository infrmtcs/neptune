#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

const int BUFF = 10000;

int main(int argc, char** argv){
    if (argc<3){
        printf("Not enough parameters (need 2)\n");
        return 1;
    }
    
    char * fname = (char*)argv[1];
    int n = atoi(argv[2]);
    
    int file1 = open(fname, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    
    if (file1==-1){
        perror(NULL);
        return 1;
    }
    
    int pos = lseek(file1, 0, SEEK_END), len;
    if (pos%n==0) len = pos;
    else
        len = pos - pos%n + n;
 
    char buff_i[BUFF], buff_j[BUFF];       
    int num_block = len/n;
    if (len>pos){
        memset(buff_i, ' ', sizeof buff_i);
        while (len > pos + BUFF){
            write(file1, buff_i, BUFF);
            pos = lseek(file1, 0, SEEK_END);
        }
        write(file1, buff_i, len - pos);
    }
    
    int i, j;
    for (i=0, j=num_block-1; i<j; i++, j--){
        int k;
        for (k=0; k<n; k+=BUFF){
            if (k+BUFF < n) len = BUFF;
            else
                len = n - k;
            
            lseek(file1, i*n+k, SEEK_SET);
            read(file1, buff_i, len);
            
            lseek(file1, j*n+k, SEEK_SET);
            read(file1, buff_j, len);
            
            lseek(file1, i*n+k, SEEK_SET);
            write(file1, buff_j, len);
            
            lseek(file1, j*n+k, SEEK_SET);
            write(file1, buff_i, len);
        }
    }
    
    close(file1);
    
    return 0;
}
