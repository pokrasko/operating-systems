#include <helpers.h>

typedef int fd_t;

struct buf_t {
	char* data;
	size_t capacity;
	size_t size;
};

typedef struct buf_t buf_t;

buf_t* buf_new(size_t);
void buf_free(buf_t*);
size_t buf_capacity(buf_t*);
size_t buf_size(buf_t*);
ssize_t buf_fill(fd_t, buf_t*, size_t);
ssize_t buf_fill_at_once(fd_t, buf_t*, size_t);
ssize_t buf_flush(fd_t, buf_t*, size_t);
ssize_t buf_readline(fd_t, buf_t*, char*, size_t);
