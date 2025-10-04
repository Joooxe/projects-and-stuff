#include "utf8_file.h"
#include <errno.h>
#include <unistd.h>

int utf8_write(utf8_file_t* f, const uint32_t* str, size_t count) {
    size_t i = 0;
    while (i < count) {
        uint32_t unicode_bits = str[i];
        unsigned char utf_bytes[6];
        size_t num_bytes;

        if (unicode_bits <= 0x7F) {
            utf_bytes[0] = unicode_bits & 0x7F;
            num_bytes = 1;
        }
        else if (unicode_bits <= 0x7FF) {
            utf_bytes[0] = 0xC0 | ((unicode_bits >> 6) & 0x1F); // 11000000 | (00X...X)
            utf_bytes[1] = 0x80 | (unicode_bits & 0x3F);
            num_bytes = 2;
        }
        else if (unicode_bits <= 0xFFFF) {
            utf_bytes[0] = 0xE0 | ((unicode_bits >> 12) & 0x0F);
            utf_bytes[1] = 0x80 | ((unicode_bits >> 6) & 0x3F);
            utf_bytes[2] = 0x80 | (unicode_bits & 0x3F);
            num_bytes = 3;
        }
        else if (unicode_bits <= 0x1FFFFF) {
            utf_bytes[0] = 0xF0 | ((unicode_bits >> 18) & 0x07);
            utf_bytes[1] = 0x80 | ((unicode_bits >> 12) & 0x3F);
            utf_bytes[2] = 0x80 | ((unicode_bits >> 6) & 0x3F);
            utf_bytes[3] = 0x80 | (unicode_bits & 0x3F);
            num_bytes = 4;
        }
        else if (unicode_bits <= 0x3FFFFFF) {
            utf_bytes[0] = 0xF8 | ((unicode_bits >> 24) & 0x03);
            utf_bytes[1] = 0x80 | ((unicode_bits >> 18) & 0x3F);
            utf_bytes[2] = 0x80 | ((unicode_bits >> 12) & 0x3F);
            utf_bytes[3] = 0x80 | ((unicode_bits >> 6) & 0x3F);
            utf_bytes[4] = 0x80 | (unicode_bits & 0x3F);
            num_bytes = 5;
        }
        else if (unicode_bits <= 0x7FFFFFFF) {
            utf_bytes[0] = 0xFC | ((unicode_bits >> 30) & 0x01); //11111100 | 0...0X
            utf_bytes[1] = 0x80 | ((unicode_bits >> 24) & 0x3F);
            utf_bytes[2] = 0x80 | ((unicode_bits >> 18) & 0x3F);
            utf_bytes[3] = 0x80 | ((unicode_bits >> 12) & 0x3F);
            utf_bytes[4] = 0x80 | ((unicode_bits >> 6) & 0x3F);
            utf_bytes[5] = 0x80 | (unicode_bits & 0x3F);
            num_bytes = 6;
        }
        else {
            errno = EILSEQ;
            return (i > 0) ? i : -666;
        }

        size_t total_written = 0;
        while (total_written < num_bytes) {
            ssize_t written = write(f->fd, &utf_bytes[total_written], 1);
            if (written < 0) {
                return -1; // original write errno
            }
            total_written += written;
        }

        i++;
    }
    return i;
}


int utf8_read(utf8_file_t* f, uint32_t* res, size_t count) {
    size_t i = 0;
    while (i < count) {
        unsigned char first;
        ssize_t r = read(f->fd, &first, 1);

        if (r == 0) {
            return i;
        }

        if (r < 0) {  // original read errno
            return -1;
        }

        uint32_t unicode_bits = 0;
        size_t num_bytes = 0;

        if (first <= 0x7F) { // f <= (0)1111111
            unicode_bits = first;
            num_bytes = 1;
        }
        else if ((first & 0xE0) == 0xC0) { // f & 111000000 == (110)000000
            unicode_bits = first & 0x1F; // 000(11111)
            num_bytes = 2;
        }
        else if ((first & 0xF0) == 0xE0) { // f & 11110000 == (1110)0000
            unicode_bits = first & 0x0F;
            num_bytes = 3;
        }
        else if ((first & 0xF8) == 0xF0) {
            unicode_bits = first & 0x07;
            num_bytes = 4;
        }
        else if ((first & 0xFC) == 0xF8) {
            unicode_bits = first & 0x03;
            num_bytes = 5;
        }
        else if ((first & 0xFE) == 0xFC) {
            unicode_bits = first & 0x01;
            num_bytes = 6;
        }
        else {
            errno = EILSEQ; // ошибка посл-ти байтов
            return (i > 0) ? i : -666;
        }

        for (size_t b = 1; b < num_bytes; b++) {
            unsigned char next_byte;
            r = read(f->fd, &next_byte, 1);
            if (r == 0 || (next_byte & 0xC0) != 0x80) {
                errno = EILSEQ;
                return (i > 0) ? i : -666;
            }
            unicode_bits = (unicode_bits << 6) | (next_byte & 0x3F); // X...X000000 | (00XXXXXX) = X...X
        }

        res[i] = unicode_bits;
        i++;
    }
    return i;
}

utf8_file_t* utf8_fromfd(int fd) {
    utf8_file_t* f = malloc(sizeof(utf8_file_t*));
    if (f == NULL) {
        return NULL;
    }
    f->fd = fd;
    return f;
}
