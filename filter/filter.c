#include <helpers.h>

int main(int argc, char** argv)
{
	if (argc < 2) {
		error("Use format: filter <command>");
	}

	char** args;
	char buf[4096];
	char* command;
	int i, result;

	args = malloc(argc * sizeof(char*));
	for (i = 1; i < argc; ++i) {
		args[i - 1] = argv[i];
	}
	args[argc - 1] = buf;
	command = args[0];

	while ((result = read_until(STDIN_FILENO, buf, sizeof(buf), '\n')) > 0) {
		if (buf[result - 1] == '\n') {
			buf[result - 1] = '\0';
		}
		if (spawn(command, args) == 0) {
			write_(STDOUT_FILENO, buf, result);
			printf("\n");
		}
	}
}
