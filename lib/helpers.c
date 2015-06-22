#include <helpers.h>

void error(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

int read__(int fd, void* buf, size_t count1, size_t count2)
{
	ssize_t icount = count2;
	ssize_t result;

	do {
		result = read(fd, buf + count2 - icount, icount);
		if (result == -1) {
			return icount - count2;
		}
		icount -= result;
	} while (count2 - icount < count1 && icount > 0 && result > 0);

	return count2 - icount;
}

ssize_t read_(int fd, void *buf, size_t count)
{
	int result = read__(fd, buf, count, count);
	return (result < 0) ? -1 : result;
}

ssize_t read_until(int fd, void *buf, size_t count, char delimiter)
{
    size_t icount = 0;
    int found, i, result;

	found = 0;
    do {
		for (i = 0; i < result; ++i) {
			if (((char*) buf)[icount + i] == delimiter) {
				found = 1;
				break;
			}
		}
		if (found) {
			break;
		}

		result = read(fd, buf + icount, count - icount);
		icount += result;
		if (result == -1) {
			return -1;
		}
	} while (icount < count && result > 0);

    return icount;
}

int spawn(const char* file, char* const argv[])
{
	pid_t pid = fork();
	if (pid != 0) {
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			//printf("return -1\n");
			return -1;
		}
		if (WIFEXITED(status)) {
			//printf("return %d\n", status);
			return status;
		} else {
			//printf("return -1\n");
			return -1;
		}
	} else {
		int null = open("/dev/null", O_WRONLY);
		if (dup2(null, STDOUT_FILENO) == -1) {
			return -1;
		}
		if (dup2(null, 2) == -1) {
			return -1;
		}

		execvp(file, argv);
		//printf("return -1\n");
		return -1;
	}
}

void thiserror()
{
	error(strerror(errno));
}

int write__(int fd, const void* buf, size_t count1, size_t count2)
{
	ssize_t icount = count2;
	ssize_t result;

	do {
		result = write(fd, buf + count2 - icount, icount);
		if (result == -1) {
			return icount - count2;
		}
		icount -= result;
	} while (count2 - icount < count1 && icount > 0 && result > 0);
	
	return count2 - icount;
}

ssize_t write_(int fd, const void *buf, size_t count)
{
	ssize_t result = write__(fd, buf, count, count);
	return (result < 0) ? -1 : result;
}

int exec(struct execargs_t* args)
{
	return spawn(args->words[0], args->words);
}

int runpiped(execargs_t** programs, size_t n)
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	struct sigaction sc, si;
	sigaction(SIGCHLD, NULL, &sc);
	sigaction(SIGINT, NULL, &si);

	pid_t pids[n];
	int pipes[n - 1][2];

	for (size_t i = 0; i < n - 1; ++i) {
		if (pipe2(pipes[i], O_CLOEXEC) == -1) {
			for (size_t j = 0; j < i; ++j) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}
			return -1;
		}
	}

	int alive = 0;
	for (int i = 0; i < n; ++i) {
		pids[i] = fork();
		if (pids[i] == 0) {
			sigprocmask(SIG_UNBLOCK, &mask, NULL);
			sigaction(SIGINT, &si, NULL);
			sigaction(SIGCHLD, &sc, NULL);

			if (i != 0) {
				dup2(pipes[i - 1][0], 0);
			}
			if (i < n - 1) {
				dup2(pipes[i][1], 1);
			}
			for (size_t j = 0; j < n - 1; ++j) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			execvp(programs[i]->words[0], programs[i]->words);
			exit(-1);
		} else {
			++alive;
		}
	}
	
	for (size_t j = 0; j < n - 1; ++j) {
		close(pipes[j][0]);
		close(pipes[j][1]);
	}

	siginfo_t info;
	while (alive) {
		if (sigwaitinfo(&mask, &info) == -1) {
			continue;
		}
		if (info.si_signo == SIGCHLD) {
			--alive;
		} else {
			for (size_t i = 0; i < n; ++i) {
				kill(pids[i], SIGKILL);
			}
			break;
		}
	}

	sigaction(SIGINT, &si, NULL);
	sigaction(SIGCHLD, &sc, NULL);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);

	return 0;
}
