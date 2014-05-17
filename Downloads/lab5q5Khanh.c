#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

const int BUFF_SIZE = 10000;

int main(int argc, char** argv){
    if (argc<4){
        printf("Not enough parameters (need 3)\n");
        return 1;
    }        
    
    char* fname = (char*)argv[1];
    char* pat = (char*)argv[2];
    char* rep_by = (char*)argv[3];
    
    int pat_len = strlen(argv[2]);
    int rep_len = strlen(argv[3]);
    char tmp_name[] = "TXXXXXX";
    
    int file1 = open(fname, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (file1==-1){
        perror(NULL);
        return 1;
    }     
    int tmp_file = mkstemp(tmp_name);
    
    int pos = 0, m = 0, len = 0;
    char buff[BUFF_SIZE], iobuff[BUFF_SIZE];
    while (1){
        int len = read(file1, buff + pos, BUFF_SIZE - pos);
        if (len == 0) break;
        pos += len;
        
        int i;
        for (i=0; i+pat_len <= pos; i++){
            if (strncmp(buff + i, pat, pat_len) == 0){
                i = i + pat_len - 1;
                write(tmp_file, iobuff, m);
                m = 0;
                write(tmp_file, rep_by, rep_len);
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
    ftruncate(file1, 0);
    lseek(tmp_file, 0, SEEK_SET);
    lseek(file1, 0, SEEK_SET);
    while (1){
        int len = read(tmp_file, buff, BUFF_SIZE);
        if (len == 0) break;
        write(file1, buff, len);
    }  
    
    close(file1);
    close(tmp_file);
    remove(tmp_name);
    return 0;
}
