#include <bufio.h>

struct buf_t* buf_new(size_t capacity)
{
	struct buf_t* result = (struct buf_t*) malloc(sizeof(struct buf_t));
	if (result == NULL) {
		return NULL;
	}
	result->size = 0;
	result->capacity = capacity;
	void* data = malloc(capacity);
	if (data == NULL) {
		free(result);
		return NULL;
	}
	result->data = data;
	return result;
}

void buf_free(struct buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	free(buf->data);
	free(buf);
}

size_t buf_capacity(struct buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	return buf->capacity;
}

size_t buf_size(struct buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	return buf->size;
}

ssize_t buf_fill(fd_t fd, struct buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (buf->size < required) {
		ssize_t result = read__(fd, buf->data + buf->size, required - buf->size);
		buf->size += abs(result);
		return (result < 0) ? -1 : result;
	} else {
		return buf->size;
	}
}

ssize_t buf_flush(fd_t fd, struct buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (required > buf->size) {
		required = buf->size;
	}
	ssize_t result = write__(fd, buf->data, required);
	buf->size -= abs(result);
	memcpy(buf->data, buf->data + abs(result), buf->size);
	return (result < 0) ? -1 : result;
}
