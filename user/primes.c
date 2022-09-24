//
// Created by lqy on 2022/9/23.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(2, "usage: primes\n", 15);
        exit(0);
    }

    close(0);
    int p[35];
    int prime[35];
    memset(prime, 0 ,sizeof(prime));
    dup(p[34]);
    pipe(p + 33);
    int i = 0;
    int num = 0;
    int min_num = 0;
    for (i = 2; i <= 35; ++i) {
        write(p[34], &i, 4);
    }

    for (i = 33; i >= 0; --i) {
        if (fork() == 0) {
            if (read(p[i], &min_num, 4) == 0) {
                exit(0);
            }
            prime[min_num] = 1;

            int cnt = 0;
            int tmp[35];
            while (read(p[i], &num, 4) != 0) {
                if (num % min_num != 0) {
                    tmp[cnt++] = num;
                }
            }
            pipe(p + i - 1);
            for (int j = 0; j < cnt; ++j) {
                write(p[i], tmp + j, 4);
            }
            close(p[i]);
            exit(0);
        }
    }
    char buf[1];
    while(read(p[2], buf, 0) != 0);
    for (i = 2; i <= 35; ++i) {
        if (prime[i]) {
            fprintf(1, "prime %d\n", i);
        }
    }

    exit(0);
}