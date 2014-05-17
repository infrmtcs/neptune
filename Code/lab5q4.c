#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int i;
int BUFF_SIZE;
int l,l2;

int main(int argc,char *argv[]){
    //the result will ignore the end of file character so that the result will not be messy 
    BUFF_SIZE = atoi(argv[2]);
    char s[BUFF_SIZE+1];
    memset(s,0,sizeof(s));
    int in_fd = open(argv[1], O_RDWR);
    if (in_fd < 0)
    {
        //if (errno == EEXIST)
        printf("file doesn't exist");
        return 1;
    }
    int out_fd = open("temp.txt", O_WRONLY|O_CREAT,S_IRWXU | S_IRWXG | S_IRWXO);


    //seek to end of new file size and append new space
    int filelen = lseek(in_fd,0,SEEK_END)-1;
    lseek(in_fd,-1,SEEK_CUR);
    if (filelen % BUFF_SIZE != 0)
    {
        while (filelen % BUFF_SIZE != 0) 
        {
            write(in_fd," ",1);
            filelen++;
        }
    }
    filelen = lseek(in_fd,0,SEEK_CUR);

    //read s from beginning of file 
    //write s from the end of temp
    lseek(in_fd,0,SEEK_SET);
    lseek(out_fd,filelen,SEEK_SET);
    memset(s,0,sizeof(s));
    while ((l = read(in_fd,s,BUFF_SIZE)) > 0)
    {
        if (lseek(out_fd,-BUFF_SIZE,SEEK_CUR) < 0) break;
        write(out_fd,s,l);
        lseek(out_fd,-l,SEEK_CUR);
        memset(s,0,sizeof(s));
    }
    truncate("temp.txt",filelen);
    //write back 
    close(in_fd);
    close(out_fd);
    in_fd = open("temp.txt", O_RDONLY);
    out_fd = open(argv[1], O_WRONLY);
    memset(s,0,sizeof(s));
    while ((l = read(in_fd,s,BUFF_SIZE)) > 0){
        write(out_fd,s,l);
        memset(s,0,sizeof(s));
    }
    close(in_fd);
    close(out_fd);
    return 0;
}

