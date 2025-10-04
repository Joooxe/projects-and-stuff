#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bits/fcntl-linux.h>
#include <sys/stat.h>

const int BUFFER_SIZE = 4096;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }

    const char *filename = argv[1];
    int file = open(filename, O_RDONLY);
    if (file < 0) {
        return -1;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = read(file, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_to_write = (long)bytes_read;
        ssize_t bytes_written_total = 0;
        while (bytes_to_write > 0) {
            ssize_t bytes_written = write(1, buffer + bytes_written_total, bytes_to_write);
            if (bytes_written < 0) {
                close(file);
                return -1;
            }
            bytes_written_total += bytes_written;
            bytes_to_write -= bytes_written;
        }
    }

    off_t current_pos = lseek(file, 0, SEEK_CUR);
    if (current_pos < 0) {
        close(file);
        return -1;
    }
    while (1) {
        struct stat st;
        if (stat(filename, &st)) {
            close(file);
            return -1;
        }

        if (st.st_size > current_pos) {
            bytes_read = read(file, buffer, BUFFER_SIZE);
            if (bytes_read == 0) {
                close(file);
                return -1;
            }

            ssize_t bytes_to_write = (long)bytes_read;
            ssize_t bytes_written_total = 0;
            while (bytes_to_write > 0) {
                ssize_t bytes_written = write(1, buffer + bytes_written_total, bytes_to_write);
                if (bytes_written < 0) {
                    close(file);
                    return -1;
                }
                bytes_written_total += bytes_written;
                bytes_to_write -= bytes_written;
            }

            current_pos += (long)bytes_read;
        }
    }

    close(file);
    return 0;
}
