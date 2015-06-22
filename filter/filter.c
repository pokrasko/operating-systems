#define SIZE 4096

#include <bufio.h>

int main(int argc, char** argv)
{
	if (argc < 2) {
		error("Use format: filter <command>");
	}

	char** args;
	buf_t* buf = buf_new(SIZE);
	char str[4096];
	char* command;
	int i, result;

	//printf("1\n");
	args = malloc((argc + 1) * sizeof(char*));
	for (i = 1; i < argc; ++i) {
		args[i - 1] = argv[i];
	}
	args[argc - 1] = str;
	args[argc] = NULL;
	command = args[0];

	//printf("2\n");
	while ((result = buf_readline(STDIN_FILENO, buf, str, SIZE)) > 0) {
		//printf("result = %d\n", result);
		if (spawn(command, args) == 0) {
			write_(STDOUT_FILENO, str, result);
			printf("\n");
		}
	}

	return 0;
}
