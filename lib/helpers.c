#include <helpers.h>

void error(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int read__(int fd, void* buf, size_t count)
{
	ssize_t icount = count;
	ssize_t result;

	do {
		result = read(fd, buf + count - icount, icount);
		if (result == -1) {
			return icount - count;
		}
		icount -= result;
	} while (icount > 0 && result > 0);

	return count - icount;
}

ssize_t read_(int fd, void *buf, size_t count)
{
    int result = read__(fd, buf, count);
	return (result < 0) ? -1 : result;
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

int spawn(const char* file, char* const argv[])
{
	pid_t pid = fork();
	if (pid != 0) {
		int status;
		if (wait(&status) != -1) {
			return status;
		} else {
			return -1;
		}
	} else {
		execvp(file, argv);
		return -1;
	}
}

void thiserror()
{
	error(strerror(errno));
}

int write__(int fd, const void* buf, size_t count)
{
	ssize_t icount = count;
	ssize_t result;

	do {
		result = write(fd, buf + count - icount, icount);
		if (result == -1) {
			return icount - count;
		}
		icount -= result;
	} while (icount > 0 && result > 0);
	
	return count - icount;
}

ssize_t write_(int fd, const void *buf, size_t count)
{
	ssize_t result = write__(fd, buf, count);
	return (result < 0) ? -1 : result;
}
