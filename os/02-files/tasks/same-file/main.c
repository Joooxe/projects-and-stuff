#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

bool is_same_file(const char* lhs_path, const char* rhs_path) {
    struct stat lhs_stat;
    struct stat rhs_stat;
    stat(lhs_path, &lhs_stat);
    stat(rhs_path, &rhs_stat);

    if (lhs_stat.st_nlink != 0 && rhs_stat.st_nlink != 0) {
        return lhs_stat.st_ino == rhs_stat.st_ino && lhs_stat.st_dev == rhs_stat.st_dev;
    }
    return false;
}

int main(int argc, const char* argv[]) {
    if (argc != 3) {
        return 1;
    }

    const char* path1 = argv[1];
    const char* path2 = argv[2];

    if (is_same_file(path1, path2)) {
        printf("yes\n");
    } else {
        printf("no\n");
    }

    return 0;
}
