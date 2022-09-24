//
// Created by lqy on 2022/9/23.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "usage: pingpong\n", 15);
        exit(0);
    }

    int p[2];
    pipe(p);
    close(0);
    dup(p[0]);
    if (fork() == 0) {
        char buf[1];
        read(p[0], buf, 1);
        fprintf(1, "%d: received ping\n", getpid());
        write(p[1], "p", 1);
        close(p[1]);
    }
    else {
        char buf1[1];
        write(p[1], "p", 1);
        read(p[0], buf1, 1);
        fprintf(1, "%d: received pong\n", getpid());
        close(p[0]);
    }
    exit(0);
}