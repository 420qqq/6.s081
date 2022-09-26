//
// Created by lqy on 2022/9/23.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void f(int fd, int times) {
    if (times >= 35) {
        return;
    }
    int p[2];
    pipe(p);
    if (fork() == 0) {
        close(p[0]);
        int num = 0;
        int min_num = 0;
        if (read(fd, &min_num, 4) == 0) {
            exit(0);
        }
        if (min_num == 0) {
            exit(0);
        }
        fprintf(1, "prime %d\n", min_num);

        while (read(fd, &num, 4) != 0) {
            if (num % min_num != 0) {
                write(p[1], &num, 4);
            }
        }
        close(p[1]);
        exit(0);
    }
    else {
        close(p[1]);
        f(p[0], times + 1);
        close(p[0]);
    }
}

int main (int argc, char *argv[]) {
    if (argc != 1) {
        exit(0);
    }

    int p[2];
    pipe(p);
    int i = 0;
    for (i = 2; i <= 35; ++i) {
        write(p[1], &i, 4);
    }

    f(p[0], 1);

    sleep(1);

    exit(0);
}