#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int i;
#define BUFF_SIZE 32
char s[BUFF_SIZE+1];
char newname[BUFF_SIZE+1];
char dirname[BUFF_SIZE+1];
char fname1[BUFF_SIZE+1];
char fname2[BUFF_SIZE+1];
int l;

int find(char* s, int n, char ch)
{
    int i  =0;
    for (i = 0; i < n; i++)
        if (s[i] == ch)
            return i;
    return -1;
}

int readAtPos(int fd, char * s, int pos)
{
    lseek(fd, pos, SEEK_SET);
    return read(fd, s, BUFF_SIZE);
}

int writeAtPos(int fd, char * s, int pos)
{
    lseek(fd, pos, SEEK_SET);
    return write(fd, s, BUFF_SIZE);
}

int main(int argc,char *argv[]){
    //cout<<argc<<' '<<argv[0]<<endl;
    int inout_fd = open(argv[1], O_RDWR);
    if (inout_fd < 0)
    {
        if (errno == EEXIST)
        {
            printf("file doesn't exist");
            return 0;
        }
    }
    //read first line
    //file name is small, so we just read 1 block
    l = read(inout_fd,s,BUFF_SIZE);
    int pos = find(s,l,'\n');
    memcpy( newname, &s, pos );

    //copy content
    int cur1, cur2;
    cur1 = 0;
    cur2 = pos+1;
    int sumsize = 0;
    memset(s,0,sizeof(s));
    while ((l = readAtPos(inout_fd,s,cur2)) > 0){
        writeAtPos(inout_fd,s,cur1);
        cur1 += l;
        cur2 += l;
        sumsize += l;
        memset(s,0,sizeof(s));
    }
    close(inout_fd);
    //rename
    rename(argv[1],newname);
    truncate(newname,sumsize);

    return 0;
}
