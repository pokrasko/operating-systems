#include <bufio.h>

buf_t* buf_new(size_t capacity)
{
	buf_t* result = (struct buf_t*) malloc(sizeof(struct buf_t));
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

void buf_free(buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	free(buf->data);
	free(buf);
}

size_t buf_capacity(buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	return buf->capacity;
}

size_t buf_size(buf_t* buf)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	return buf->size;
}

ssize_t buf_fill(fd_t fd, buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (buf->size < required) {
		ssize_t result = read__(fd, buf->data + buf->size, required - buf->size, buf->capacity - buf->size);
		buf->size += abs(result);
		return (result < 0) ? -1 : result;
	} else {
		return buf->size;
	}
}

ssize_t buf_fill_at_once(fd_t fd, buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (buf->size < required) {
		ssize_t result = read(fd, buf->data + buf->size, required - buf->size);
		if (result == -1) {
			return -1;
		}
		buf->size += result;
		return result;
	} else {
		return 0;
	}
}

ssize_t buf_flush(fd_t fd, buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (required > buf->size) {
		required = buf->size;
	}
	ssize_t result = write__(fd, buf->data, required, buf->size);
	buf->size -= abs(result);
	memcpy(buf->data, buf->data + abs(result), buf->size);
	return (result < 0) ? -1 : result;
}

ssize_t buf_flush_at_once(fd_t fd, buf_t* buf, size_t required)
{
#ifdef DEBUG
	if (buf == NULL) {
		abort();
	}
#endif

	if (required > buf->size) {
		required = buf->size;
	}
	ssize_t result = write(fd, buf->data, required);
	if (result == -1) {
		return -1;
	}
	buf->size -= result;
	memcpy(buf->data, buf->data + result, buf->size);
	return result;
}

ssize_t buf_readline(fd_t fd, buf_t* buf, char* str, size_t required)
{
	size_t offset = 0;
	size_t result;
	int found = 0;

	//printf("fuck\n");
	do {
		size_t i;
		for (i = offset; i < buf->size; ++i) {
			if (buf->data[i] == '\n') {
				found = 1;
				break;
			}
		}
		if (found) {
			//printf("buf->size = %d, found symbol number = %d\n", buf->size, i);
			memmove(str, buf->data, i);
			buf->size -= i + 1;
			memmove(buf->data, buf->data + i + 1, buf->size);
			//printf("buf->size = %d\n", buf->size);
			return i;
		}
		offset = buf->size;

		result = read(fd, buf->data, required - buf->size);
		buf->size += result;
		//printf("result = %d\n", result);
		if (result == -1) {
			return -1;
		}
	} while (buf->size < required && result > 0);

	memmove(str, buf->data, buf->size);
	result = buf->size;
	buf->size = 0;
	//printf("result = %d\n", result);
	return result;
}

ssize_t buf_readline_(buf_t* buf, char* str, size_t size) {
	for (size_t i = 0; i < buf->size; ++i) {
		if (buf->data[i] == '\n') {
			memmove(str, buf->data, i);
			buf->size -= i + 1;
			memmove(buf->data, buf->data + i + 1, buf->size);
			return i;
		}
	}

	return 0;
}

ssize_t buf_writeline(buf_t* buf, char* str, size_t size) {
	if (size > buf->capacity - buf->size) {
		return -1;
	}

	memmove(buf->data + buf->size, str, size);
	buf->size += size;
	return 0;
}
