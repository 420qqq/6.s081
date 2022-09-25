//
// Created by lqy on 2022/9/23.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    exit(0);
    if (argc != 1) {
        fprintf(2, "usage: primes\n", 15);
        exit(0);
    }

    int p[70];
    int prime[35];
    memset(prime, 0 ,sizeof(prime));
    pipe(p + 68);
    int i = 0;
    int num = 0;
    int min_num = 0;
    for (i = 2; i <= 35; ++i) {
        write(p[69], &i, 4);
    }
    close(p[69]);

    for (i = 33; i >= 0; --i) {
        if (fork() == 0) {
            if (read(p[2*i+2], &min_num, 4) == 0) {
                exit(0);
            }
            prime[min_num] = 1;

            int cnt = 0;
            int tmp[35];
            while (read(p[2*i+2], &num, 4) != 0) {
                if (num % min_num != 0) {
                    tmp[cnt++] = num;
                }
            }
            if (pipe(p + i - 1) == -1) {
                fprintf(2, "failed to create pipe\n", 30);
                exit(0);
            }
            for (int j = 0; j < cnt; ++j) {
                write(p[2*i+1], tmp + j, 4);
            }
            close(p[2*i+1]);
            close(p[2*i+2]);
            if (i == 0) { close(p[0]); }
            exit(0);
        }
    }
    char buf[1];
    while(read(p[0], buf, 0) != 0);
    for (i = 2; i <= 35; ++i) {
        if (prime[i] == 1) {
            fprintf(1, "prime %d\n", i);
        }
    }

    exit(0);
}