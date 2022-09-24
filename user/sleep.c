//
// Created by lqy on 2022/9/23.
//

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "usage: sleep [time]\n");
    }
    else {
        sleep(atoi(argv[1]));
    }
    exit(0);
}