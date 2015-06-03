#include <helpers.h>

void error(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
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
		icount += result;
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
