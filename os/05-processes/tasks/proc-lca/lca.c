#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include "lca.h"


static pid_t get_parent_pid(pid_t pid) {
    char path[64];
    sprintf(path, "/proc/%d/stat", pid);

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    char buf[4096];
    ssize_t n = read(fd, buf, sizeof(buf));
    close(fd);
    if (n <= 0) {
        return -1;
    }

    int pid_read;
    int ppid;
    char comm[1024];
    char state;
    sscanf(buf, "%d %s %c %d", &pid_read, comm, &state, &ppid);
    return ppid;
}

pid_t find_lca(pid_t x, pid_t y) {
    if (x == y) {
        return x;
    }
    pid_t ancestors_x[MAX_TREE_DEPTH];
    pid_t ancestors_y[MAX_TREE_DEPTH];
    int len_x = 0;
    int len_y = 0;

    pid_t cur = x;
    while (cur > 0 && len_x < MAX_TREE_DEPTH) {
        ancestors_x[++len_x] = cur;
        pid_t p = get_parent_pid(cur);
        cur = p;
    }
    cur = y;
    while (cur > 0 && len_y < MAX_TREE_DEPTH) {
        ancestors_y[++len_y] = cur;
        pid_t p = get_parent_pid(cur);
        cur = p;
    }

    int i = len_x - 1;
    int j = len_y - 1;

    pid_t lca = 1;
    while (i >= 0 && j >= 0 && ancestors_x[i] == ancestors_y[j]) {
        lca = ancestors_x[i];
        i--;
        j--;
    }

    return lca;
}
