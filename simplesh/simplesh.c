#include "bufio.h"

#define LIMIT 4096

execargs_t* parse_args(const char* str, size_t size)
{
	size_t count = 0;
	size_t i;
	int is_space = 1;
	for (size_t i = 0; i < size; ++i) {
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
	execargs_t* result = (struct execargs_t*) malloc(sizeof(struct execargs_t));
	result->words = (char**) malloc((count + 1) * sizeof(char*));
	memset(result->words + count * sizeof(char*), 0, sizeof(char*));
	result->size = count;

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
				char* arg = (char*) malloc(i - begin + 1);
				arg[i - begin] = 0;
				memcpy(arg, str + begin, i - begin);
				result->words[count++] = arg;
			}
			is_space = 1;
		}
	}
	if (is_space == 0) {
		char* arg = (char*) malloc(size - begin + 1);
		arg[size - begin] = 0;
		memcpy(arg, str + begin, size - begin);
		result->words[count] = arg;
	}

	return result;
}

execargs_t** parse_commands(const char* str, size_t size, size_t* number)
{
	size_t count = 1;
	for (size_t i = 0; i < size; ++i) {
		if (str[i] == '|') {
			++count;
		}
	}

	execargs_t** result = (struct execargs_t**) malloc((count + 1) * sizeof(struct execargs_t*));
	memset(result + count, 0, sizeof(execargs_t*));

	size_t offset = 0;
	count = 0;
	for (size_t i = 0; i < size; ++i) {
		if (str[i] == '|') {
			char* command = (char*) malloc((i + 1 - offset) * sizeof(char));
			command[i - offset] = 0;
			memcpy(command, str + offset, i - offset);
			result[count] = parse_args(command, i - offset);
			++count;
			offset = i + 1;
		}
	}
	char* command = (char*) malloc((size + 1 - offset) * sizeof(char));
	command[size - offset] = 0;
	memcpy(command, str + offset, size - offset);
	result[count] = parse_args(command, size - offset);
	++count;

	*number = count;
	return result;
}

int main()
{
	buf_t* buf = buf_new(LIMIT);
	char str[LIMIT];
	char welcome[1] = "$";
	ssize_t result;
	size_t count;

	write_(STDOUT_FILENO, welcome, 1);
	while ((result = buf_readline(STDIN_FILENO, buf, str, LIMIT)) > -2) {
		execargs_t** programs = parse_commands(str, result, &count);
		runpiped(programs, count);
		write_(STDOUT_FILENO, welcome, 1);
	}

	buf_free(buf);

	return 0;
}
