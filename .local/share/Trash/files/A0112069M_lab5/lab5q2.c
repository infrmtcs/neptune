#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

int i;
#define BUFF_SIZE 8
char s[BUFF_SIZE+1];
int l;

int main(int argc,char *argv[]){
    int inout_fd = open(argv[1], O_RDWR);
    if (inout_fd < 0)
    {
        if (errno == EEXIST)
        {
            printf("file doesn't exist");
            return 0;
        }
    }

    int ok = 1;

    memset(s,0,sizeof(s));
    while ((l = read(inout_fd,s,BUFF_SIZE)) > 0){
        for (i = 0; i < strlen(s); i++)
        {
            if (isalpha(s[i]))
            {
                if (islower(s[i]))
                    s[i] = toupper(s[i]);
                else
                    s[i] = tolower(s[i]);
            }
        }
        lseek(inout_fd,-l,SEEK_CUR);
        write(inout_fd,s,l);
        memset(s,0,sizeof(s));
    }
    close(inout_fd);
    return 0;
}
