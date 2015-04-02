#include <bufio.h>

int main()
{
	struct buf_t* buf = buf_new(4096);
	ssize_t result, written;

	do {
		result = buf_fill(STDIN_FILENO, buf, buf_capacity(buf));
		if (result == -1) {
			thiserror();
		}
		written = buf_flush(STDOUT_FILENO, buf, result);
		if (written < result) {
			error("Unexpected output error");
		}
	} while (result > 0);

	return 0;
}
