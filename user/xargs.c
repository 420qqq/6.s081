//
// Created by lqy on 2022/9/24.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main (int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "usage: xargs [command] [args]\n");
        exit(0);
    }

    char buf[512];
    char *args[MAXARG];
    for (int i=0; i < MAXARG; ++i) {
        args[i] = (char *) malloc(512);
    }
    int ori_cnt = argc;
    for (int i = 1; i < argc; ++i) {
        strcpy(args[i - 1], argv[i]);
    }
    int cnt = ori_cnt;
    int len = 0;

    while ((len = read(0, buf, sizeof(buf))) != 0) {
        buf[len] = 0;
        char *p = buf;
        char *last = buf;
        while (p) {
            fprintf(1, "1");
            if ((*p) == 0) {
                // to be extended
                break;
            }
            if ((*p) == '\n') {
                if (p == last) {
                    ++p;
                    ++last;
                    continue;
                }
                (*p) = 0;
                strcpy(args[cnt++], last);
                last = ++p;
                args[cnt] = 0;
                if (fork() == 0) {
                    sleep(1);
                    exec(args[0], args);
                }
                else {
                    wait((int *) 0);
                    cnt = ori_cnt;
                }
            }

            if ((*p) == ' ') {
                (*p) = 0;
                strcpy(args[cnt++], last);
                last = ++p;
            }
            else {
                ++p;
            }
        }
    }

    exit(0);
}