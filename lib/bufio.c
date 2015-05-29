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

ssize_t buf_readline(fd_t fd, struct buf_t* buf, char* str, size_t limit)
{
	int eol_found = -1;
	int eof_found = 0;
	while (buf->size < buf->capacity) {
		for (size_t i = 0; i < buf->size; ++i) {
			if (buf->data[i] == '\n') {
				eol_found = i;
				break;
			}
		}
		if (eol_found >= 0) {
			break;
		}
		ssize_t result = read_until(fd, buf->data + buf->size, buf->capacity - buf->size, '\n');
		if (result == -1) {
			return -1;
		} else if (result == 0) {
			eof_found = 1;
			break;
		}
		buf->size += result;
	}

	if (eol_found >= 0 || limit < buf->size || eof_found) {
		if (eof_found) {
			return -2;
		} else if (limit < eol_found) {
			eol_found = limit;
		} else if (eof_found) {
			eol_found = buf->size;
		}
		memmove(str, buf->data, eol_found);
		if (eol_found < buf->size && buf->data[eol_found] == '\n') {
			--buf->size;
		}
		buf->size -= eol_found;
		memmove(buf->data, buf->data + eol_found, buf->size);
		return eol_found;
	} else {
		memmove(str, buf->data, buf->size);
		ssize_t result = buf->size;
		str += result;
		limit -= result;
		buf->size = 0;
		ssize_t recurresult = buf_readline(fd, buf, str, limit);
		if (recurresult == -1) {
			return -1;
		} else {
			return result + recurresult;
		}
	}
}
