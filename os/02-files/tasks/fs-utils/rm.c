#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <dirent.h>

void remove_dir(const char *path) {
    struct stat path_stat;
    lstat(path, &path_stat);

    if (S_ISDIR(path_stat.st_mode)) {
        DIR *dir = opendir(path);
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            char full_path[PATH_MAX];
            strcpy(full_path, path);
            strcat(full_path, "/");
            strcat(full_path, entry->d_name);
            remove_dir(full_path);
        }

        closedir(dir);
        rmdir(path);
    } else {
        unlink(path);
    }
}

int main(int argc, char *argv[]) {
    int opt;
    int recursive = 0;

    while ((opt = getopt(argc, argv, "r")) != -1) {
        if (opt == 'r') {
            recursive = 1;
        } else {
            exit(1);
        }
    }

    for (int i = optind; i < argc; i++) {
        struct stat path_stat;
        lstat(argv[i], &path_stat);

        if (S_ISDIR(path_stat.st_mode)) {
            if (recursive) {
                remove_dir(argv[i]);
            } else {
                exit(1);
            }
        } else {
            unlink(argv[i]);
        }
    }
    return 0;
}
