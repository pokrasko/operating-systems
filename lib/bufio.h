#include <helpers.h>
#include <stdlib.h>
#include <unistd.h>

typedef int fd_t;

struct buf_t {
	void* data;
	size_t capacity;
	size_t size;
};

struct buf_t* buf_new(size_t);
void buf_free(struct buf_t*);
size_t buf_capacity(struct buf_t*);
size_t buf_size(struct buf_t*);
ssize_t buf_fill(fd_t, struct buf_t*, size_t);
ssize_t buf_flush(fd_t, struct buf_t*, size_t);
