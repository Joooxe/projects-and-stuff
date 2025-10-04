#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <errno.h>

void make_directory(const char *path, int mode, int parents) {
    if (parents) {
        char str_path[PATH_MAX];
        strcpy(str_path, path);
        char *p = str_path + 1;
        while (*p) {
            if (*p == '/') {
                *p = 0;
                mkdir(str_path, 0755);
                if (errno != EEXIST) {
                    perror("mkdir");
                    exit(1);
                }
                *p = '/';
            }
            p++;
        }
    }

    if (mkdir(path, mode) == -1) {
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int parents = 0;
    int mode = 0755;

    struct option long_options[] = {
        {"mode", required_argument, 0, 'm'}
    };

    while ((opt = getopt_long(argc, argv, "pm:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'p':
                parents = 1;
            break;
            case 'm':
                mode = strtol(optarg, NULL, 8);
            break;
            default:
                exit(1);
        }
    }

    for (int i = optind; i < argc; i++) {
        make_directory(argv[i], mode, parents);
    }

    return 0;
}
