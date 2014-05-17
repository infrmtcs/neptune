#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 4096
char* filename;
char* pattern;
char* repst;
int pattern_len;
int rep_len;
int i;

int main(int argc, char** argv){
    
    filename = (char*)argv[1];
    pattern = (char*)argv[2];
    repst = (char*)argv[3];
    
    pattern_len = strlen(argv[2]);
    rep_len = strlen(argv[3]);
    char tmp_name[] = "TXXXXXX";
    
    int inout_fd = open(filename, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (inout_fd==-1){
        printf("file doesn't exist");
        return 1;
    }     
    int tmp_file = mkstemp(tmp_name);//make temp file
    
    int pos = 0, m = 0, len = 0;
    char buff[BUFF_SIZE], iobuff[BUFF_SIZE];
    while (1){
        int len = read(inout_fd, buff + pos, BUFF_SIZE - pos);
        if (len == 0) break;
        pos += len;
        
        for (i=0; i+pattern_len <= pos; i++){
            if (strncmp(buff + i, pattern, pattern_len) == 0){
                i = i + pattern_len - 1;
                write(tmp_file, iobuff, m);
                m = 0;
                write(tmp_file, repst, rep_len);
            }
            else {
                iobuff[m] = buff[i];
                m++;
                if (m==BUFF_SIZE){
                    write(tmp_file, iobuff, m);
                    m = 0;
                }
            }
        }
        
        if (i<pos){
            for (len = i; len<pos; len++) buff[len - i] = buff[len];
            pos -= i;
        }
        else
            pos = 0;
    }
    write(tmp_file, iobuff, m);
    write(tmp_file, buff, pos);
    truncate(filename, 0);
    lseek(tmp_file, 0, SEEK_SET);
    lseek(inout_fd, 0, SEEK_SET);
    while (1){
        int len = read(tmp_file, buff, BUFF_SIZE);
        if (len == 0) break;
        write(inout_fd, buff, len);
    }  
    
    close(inout_fd);
    close(tmp_file);
    remove(tmp_name);
    return 0;
}
