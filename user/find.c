//
// Created by lqy on 2022/9/24.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path)
{
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    return p;
}


char *strcat(char* dest, char* src) {
    int src_len = strlen(src);
    int dest_len = strlen(dest);
    for (int i = 0; i < src_len; ++i) {
        dest[i + dest_len] = src[i];
    }
    dest[src_len + dest_len] = 0;
    return dest;
}

void find(char *path, char *match) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
        case T_FILE:
            if (strcmp(fmtname(path), match) == 0){
                fprintf(1, "%s\n", fmtname(path));
            }
            break;

        case T_DIR:
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                if (strcmp(".", fmtname(de.name)) == 0 || strcmp("..", fmtname(de.name)) == 0) {
                    continue;
                }
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0){
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }
                if (st.type == T_DIR) {
                    char fuck[512];
                    strcpy(fuck, path);
                    strcat(fuck, "/");
                    find(buf, match);
                }
                else {
                    if (strcmp(fmtname(buf), match) == 0) {
                        fprintf(1, "%s/%s\n", path, fmtname(buf));
                    }
                }
            }
            break;
    }
    close(fd);
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find [path] [match]\n", 15);
        exit(0);
    }

    find(argv[1], argv[2]);

    exit(0);
}