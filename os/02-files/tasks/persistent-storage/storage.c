#include "storage.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>

int make_directory(const char *path, int mode) {
    char str_path[PATH_MAX];
    strcpy(str_path, path);
    char *p = str_path + 1;
    while (*p) {
        if (*p == '/') {
            *p = 0;
            if (mkdir(str_path, 0755) == -1) {
                //errno = ENOSPC; // no space (?)
            }
            *p = '/';
        }
        ++p;
    }

    mkdir(str_path, mode);
}

void parse_key_path(storage_t* storage, storage_key_t key, char* path_buffer, size_t buffer_size) {
    size_t len = strlen(key);
    size_t blocks_cnt = len / SUBDIR_NAME_SIZE;
    size_t blocks_remain = len % SUBDIR_NAME_SIZE;
    int pos = sprintf(path_buffer, "%s", storage->root_path);
    path_buffer[pos] = '/';
    path_buffer[++pos] = '\0';

    for (size_t i = 0; i < blocks_cnt; ++i) {
        strncpy(path_buffer + pos, key + i * SUBDIR_NAME_SIZE, SUBDIR_NAME_SIZE);
        pos += (int)SUBDIR_NAME_SIZE;
        path_buffer[pos] = '/';
        path_buffer[++pos] = '\0';
    }

    if (blocks_remain > 0) {
        strncpy(path_buffer + pos, key + blocks_cnt * SUBDIR_NAME_SIZE, blocks_remain);
        pos += (int)blocks_remain;
    } else {
        strcat(path_buffer, "@");
        pos += 1;
    }

    path_buffer[pos] = '\0';
}

ssize_t read_line(int fd, char* buffer, size_t max_length) { //  rip fgets
    ssize_t bytes_read = 0;
    ssize_t total_read = 0;
    char c;
    while (total_read < max_length - 1) {
        bytes_read = read(fd, &c, 1);
        if (bytes_read == 0) {
            break;
        }
        if (bytes_read == -1) {
            return -1;
        }

        buffer[total_read++] = c;
        if (c == '\n') {
            break;
        }
    }
    buffer[total_read] = '\0';
    return total_read;
}

void storage_init(storage_t* storage, const char* root_path) {
    storage->root_path = strdup(root_path);
}

void storage_destroy(storage_t* storage) {
    free(storage->root_path);
    storage->root_path = NULL;
}

version_t storage_set(storage_t* storage, storage_key_t key, storage_value_t value) {
    char file_path[PATH_MAX];
    parse_key_path(storage, key, file_path, sizeof(file_path));

    char dir_path[PATH_MAX];
    strcpy(dir_path, file_path);
    dir_path[PATH_MAX - 1] = '\0';


    char* last_slash = strrchr(dir_path, '/');
    *last_slash = '\0';

    if (make_directory(dir_path, 0755) == -1) {
        return -1;
    }

    int file = open(file_path, O_RDWR | O_CREAT, 0644);
    if (file == -1) {
        close(file);
        return 0;
    }

    version_t version = 0;
    char line_buffer[MAX_VALUE_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read_line(file, line_buffer, sizeof(line_buffer))) > 0) {
        version++;
    }
    if (bytes_read == -1) {
        close(file);
        return 0;
    }

    if (lseek(file, 0, SEEK_END) == -1) {
        close(file);
        return 0;
    }

    size_t value_len = strlen(value);
    char write_buffer[MAX_VALUE_SIZE];
    memcpy(write_buffer, value, value_len);
    write_buffer[value_len] = '\n';
    write_buffer[value_len + 1] = '\0';

    ssize_t bytes_written = write(file, write_buffer, value_len + 1);
    if (bytes_written != (ssize_t)(value_len + 1)) {
        close(file);
        return 0;
    }

    close(file);
    ++version;
    return version;
}

version_t storage_get(storage_t* storage, storage_key_t key, returned_value_t returned_value) {
    char file_path[PATH_MAX];
    parse_key_path(storage, key, file_path, sizeof(file_path));

    int file = open(file_path, O_RDONLY);
    if (file == -1) {
        return 0;
    }

    version_t version = 0;
    char line_buffer[MAX_VALUE_SIZE];
    char last_line[MAX_VALUE_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read_line(file, line_buffer, sizeof(line_buffer))) > 0) {
        version++;
        strncpy(last_line, line_buffer, sizeof(last_line) - 1);
        last_line[sizeof(last_line) - 1] = '\0';
    }
    if (bytes_read == -1) {
        close(file);
        return 0;
    }
    close(file);

    last_line[strlen(last_line) - 1] = '\0';
    strcpy(returned_value, last_line);

    return version;
}

version_t storage_get_by_version(storage_t* storage, storage_key_t key, version_t version, returned_value_t returned_value) {
    char file_path[PATH_MAX];
    parse_key_path(storage, key, file_path, sizeof(file_path));

    int file = open(file_path, O_RDONLY);
    if (file == -1) {
        return 0;
    }

    version_t current_version = 0;
    char line_buffer[MAX_VALUE_SIZE];
    char last_line[MAX_VALUE_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read_line(file, line_buffer, sizeof(line_buffer))) > 0) {
        current_version++;
        if (current_version == version) {
            strncpy(last_line, line_buffer, sizeof(last_line) - 1);
            last_line[sizeof(last_line) - 1] = '\0';
            break;
        }
    }
    if (bytes_read == -1) {
        close(file);
        return 0;
    }
    close(file);

    last_line[strlen(last_line) - 1] = '\0';
    strcpy(returned_value, last_line);

    return version;
}
