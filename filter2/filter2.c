#define SIZE 4096

#include <bufio.h>

char** parse(const char* str, size_t size)
{
	size_t count = 0;
	size_t i;
	int is_space = 1;
	for (i = 0; i < size; ++i) {
		if (str[i] == ' ') {
			if (is_space == 0) {
				++count;
			}
			is_space = 1;
		} else {
			is_space = 0;
		}
	}
	if (is_space == 0) {
		++count;
	}

	if (count == 0) {
		return NULL;
	}
	char** result = (char**) malloc((count + 1) * sizeof(char*));
	memset(result + count * sizeof(char*), 0, sizeof(char*));

	is_space = 1;
	count = 0;
	size_t begin;
	for (size_t i = 0; i < size; ++i) {
		if (str[i] != ' ') {
			if (is_space == 1) {
				begin = i;
			}
			is_space = 0;
		} else {
			if (is_space == 0) {
				char* res = (char*) malloc(i - begin + 1);
				res[i - begin] = 0;
				memmove(res, str + begin, i - begin);
				result[count++] = res;
			}
			is_space = 1;
		}
	}
	if (is_space == 0) {
		char* res = (char*) malloc(size - begin + 1);
		res[size - begin] = 0;
		memmove(res, str + begin, size - begin);
		result[count] = res;
	}

	return result;
}

int main(int argc, char** argv)
{
	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
	}

	buf_t* buf = buf_new(SIZE);
	char** args;
	char* command;
	char str[4096];
	int i, result;

	while ((result = buf_readline(STDIN_FILENO, buf, str, SIZE)) > 0) {
		args = parse(str, result);
		command = args[0];
		if (spawn(command, args) == 0) {
			write_(STDOUT_FILENO, str, result);
			printf("\n");
		}
	}

	return 0;
}
