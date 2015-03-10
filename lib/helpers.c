#include <helpers.h>

ssize_t read_(int fd, void *buf, size_t count)
{
    size_t icount = 0;
    size_t result;

    do {
        result = read(fd, buf + icount, icount);
        if (result == -1) {
            return -1;
        }
        icount += result;
    } while (icount < count && result > 0);

    return icount;
}

ssize_t write_(int fd, const void *buf, size_t count)
{
    size_t icount = 0;
    size_t result;

    do {
        result = write(fd, buf + icount, icount);
        if (result == -1) {
            return -1;
        }
        icount += result;
    } while (icount < count && result > 0);

    return icount;
}
