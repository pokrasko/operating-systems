#include <helpers.h>
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count)
{
    int result;
    size_t icount = count;
    while (icount > 0 && (result = read(fd, buf, icount)) > 0) {
        buf += result;
        icount -= result;
    }
    return count - icount;
}

ssize_t write(int fd, const void *buf, size_t count)
{
    int result;
    size_t icount = count;
    while (icount > 0 && (result = write(fd, buf, icount)) > 0) {
        buf += result;
        icount -= result;
    }
    return count - icount;
}
