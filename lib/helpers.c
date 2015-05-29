#include <helpers.h>

void error(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(1);
}

int read__(int fd, void* buf, size_t count)
{
	ssize_t icount = count;
	ssize_t result;

	do {
		result = read(fd, buf + count - icount, icount);
		if (result == -1) {
			return icount - count;
		}
		icount -= result;
	} while (icount > 0 && result > 0);

	return count - icount;
}

ssize_t read_(int fd, void *buf, size_t count)
{
	int result = read__(fd, buf, count);
	return (result < 0) ? -1 : result;
}

ssize_t read_until(int fd, void *buf, size_t count, char delimiter)
{
    size_t icount = 0;
    int found, i, result;

    do {
		result = read(fd, buf + icount, count - icount);
		if (result == -1) {
			return -1;
		}
		found = 0;
		for (i = 0; i < result; ++i) {
			if (((char*) buf)[icount + i] == delimiter) {
				found = 1;
				break;
			}
		}
	} while (icount < count && result > 0 && !found);

    return icount;
}

int spawn(const char* file, char* const argv[])
{
	pid_t pid = fork();
	if (pid != 0) {
		int status;
		if (wait(&status) != -1) {
			return status;
		} else {
			return -1;
		}
	} else {
		execvp(file, argv);
		return -1;
	}
}

void thiserror()
{
	error(strerror(errno));
}

int write__(int fd, const void* buf, size_t count)
{
	ssize_t icount = count;
	ssize_t result;

	do {
		result = write(fd, buf + count - icount, icount);
		if (result == -1) {
			return icount - count;
		}
		icount -= result;
	} while (icount > 0 && result > 0);
	
	return count - icount;
}

ssize_t write_(int fd, const void *buf, size_t count)
{
	ssize_t result = write__(fd, buf, count);
	return (result < 0) ? -1 : result;
}

int exec(execargs_t* args)
{
	spawn(args->words[0], args->words);
}

void endhandler(int signum)
{
	exit(0);
}

int runpiped(execargs_t** programs, size_t n)
{
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = endhandler;

	pid_t pids[n];
	int pipes[n - 1][2];

	for (int i = 0; i < n - 1; ++i) {
		if (pipe(pipes[i]) == -1) {
			return -1;
		}
	}
	for (int i = 0; i < n; ++i) {
		pids[i] = fork();
		if (pids[i] == 0) {
			if (i != 0) {
				dup2(pipes[i - 1][0], 0);
			}
			if (i < n - 1) {
				dup2(pipes[i][1], 1);
			}
			for (int j = 0; j < n - 1; ++j) {
				close(pipes[j][0]);
				close(pipes[j][1]);
			}

			if (sigaction(SIGPIPE, &sa, NULL) == -1) {
				return -1;
			}
			execvp(programs[i][0], programs[0]);
		}
	}

	sigset_t mask;
	siginfo_t info;
	sigemptyset(&mask);
	sigaddset(&mask, SIGCHLD);
	sigaddset(&mask, SIGINT);
	sigprocmask(SIG_BLOCK, &mask, NULL);

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		return -1;
	}

	while (1) {
		if (sigwaitinfo(&mask, &info) == -1) {
			continue;
		}
		switch (info.si_signo) {
		case SIGCHLD:
			for (int i = 0; i < n; ++i) {
				if (info.si_pid == pid[i]) {
					continue;
				}
				if (kill(pids[i], SIGINT) != 0) {
					return -1;
				}
			}
			return 0;
		}
	}
}
