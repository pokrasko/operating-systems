#include "bufio.h"

int main()
{
	buf_t* buf = buf_new(4096);
	char welcome[1] = "$";
	write_(FILENO_STDOUT, welcome, 1);
	while ((result = buf_fill(FILENO_STDIN, buf, sizeof(buf))) > 0) {
	}
	buf_free(buf);

	return 0;
}
