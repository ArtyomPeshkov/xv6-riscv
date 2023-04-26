#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define CHARS ((1 << 11) - 1)

void printer(char* u_buf, int len){
    int i = 0;
    while (u_buf[i] == 0 && i < len)
        i++;
    
    for (; i < len; i++){
        if (u_buf[i] != 0)
            write(1, u_buf + i, 1);
        else
            write(1, "\n", 1);
    }
}

int main(int argc, char *argv[]){
    if (argc < 4){
        write(1, "To little args\n", 14);
        exit(-1);
    }
    set_buf_settings(*(argv[1]) - '0', *(argv[2]) - '0', *(argv[3]) - '0');
    char u_buf[CHARS];
    uint64 len = dmesg(u_buf);
    if (len == -1)
        exit(-1);       
    printer(u_buf, len);
    exit(0);
}