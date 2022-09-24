//
// Created by lqy on 2022/9/24.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

char *strcat(char* src, char* dest) {
    int src_len = strlen(src);
    int dest_len = strlen(dest);
    for (int i = 0; i < src_len; ++i) {
        dest[i + dest_len] = src[i];
    }
    dest[src_len + dest_len] = 0;
    return dest;
}

struct file {
    char name[255];
    char len[20];
    char type[1];
    char num[20];
};

int read_one_item (struct file* f, int fd) {
    char buf[255];
    if (read(fd, buf, sizeof(buf)) == 0) {
        return 0;
    }
    strcpy(buf, f->name);

    return 1;
}

void find(char *path, char *match) {
    if (fork() == 0) {
        exec("ls");
    }
    else {
        struct file f;
        while (read_one_item(&f, p[0])) {
            if (strcmp(f.name, match) == 0) {
                fprintf(1, "%s/%s\n", path, f.name);
                continue;
            }
            if (strcmp(f.name, ".") == 0 || strcmp(f.name, "..") == 0) {
                continue;
            }
            if (strcmp(f.type, "1") == 0) {
                char cat[127];
                strcat("/", path);
                strcpy(path, cat);
                find(strcat(f.name, cat), match);
            }
        }
    }
    close(p[0]);
    close(p[1]);
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "usage: find [path] [match]\n", 15);
        exit(0);
    }

    find(argv[1], argv[2]);

    exit(0);
}