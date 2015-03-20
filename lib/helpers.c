#include <helpers.h>

ssize_t read_(int fd, void *buf, size_t count)
{
    size_t icount = count;
    int result;

    do {
        result = read(fd, buf + count - icount, icount);
        if (result == -1) {
            return -1;
        }
        icount -= result;
    } while (icount > 0 && result > 0);

    return count - icount;
}

ssize_t read_until(int fd, void *buf, size_t count, char delimiter)
{
    size_t icount = 0;
    int result;

    do {
        result = read(fd, buf + icount, 1);
        if (result == -1) {
            return -1;
        }
        icount += result;
    } while (icount < count && result > 0 && ((char *) buf)[icount - 1] != delimiter);

    return icount;
}

ssize_t write_(int fd, const void *buf, size_t count)
{
    size_t icount = 0;
    int result;

    do {
        result = write(fd, buf + icount, count);
        if (result == -1) {
            return -1;
        }
        icount += result;
    } while (icount < count && result > 0);

    return icount;
}
