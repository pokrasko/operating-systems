#define MAX_BUF_SIZE 4096
#define MAX_CLIENTS 127

#include <bufio.h>
#include <sys/poll.h>

struct pollfd pollfd[2 * MAX_CLIENTS + 2];
struct buf_t* buffs[2 * MAX_CLIENTS];
int opened = 0;


int create_server(char* port) {
	struct addrinfo *p, *result;
	struct addrinfo hints;
	int res, s;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, port, &hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo on port %s: %s\n", port, gai_strerror(res));
		return -1;
	}

	for (p = result; p != NULL; p = p->ai_next) {
		if ((s = socket(p->ai_family, p->ai_socktype | SOCK_CLOEXEC, p->ai_protocol)) == -1) {
			continue;
		}

		if (bind(s, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}

		close(s);
	}

	if (p == NULL) {
		fprintf(stderr, "Couldn't bind on port %s\n", port);
		return -1;
	}

	freeaddrinfo(result);

	if (listen(s, 1) == -1) {
		fprintf(stderr, "Couldn't listen on port %s\n", port);
		return -1;
	}

	printf("Socket is opened on port %s\n", port);
	return s;
}


int checkin(int fd, int buf) {
	if (pollfd[fd].revents & POLLIN
	 && buf_fill_at_once(pollfd[fd].fd, buffs[buf], buf_capacity(buffs[buf])) == -1) {
		return -1;
	} else {
		return 0;
	}
}


int checkout(int fd, int buf) {
	if (pollfd[fd].revents & POLLOUT
	 && buf_flush_at_once(pollfd[fd].fd, buffs[buf], buf_capacity(buffs[buf])) == -1) {
		return -1;
	} else {
		return 0;
	}
}


int wait_for(int server) {
	//Setting poll for server descriptors
	if (opened < MAX_CLIENTS) {
		pollfd[server].events = POLLIN;
	} else {
		pollfd[server].events = 0;
	}
	pollfd[1 - server].events = 0;

	//Setting poll for client descriptors
	for (int i = 1; i < opened + 1; ++i) {
		int size0 = buf_size(buffs[2 * i - 2]);
		int size1 = buf_size(buffs[2 * i - 1]);
		pollfd[2 * i].events = 0;
		pollfd[2 * i + 1].events = 0;

		if (size0 < MAX_BUF_SIZE) {
			pollfd[2 * i].events |= POLLIN;
		}
		if (size0 > 0) {
			pollfd[2 * i + 1].events |= POLLOUT;
		}
		if (size1 < MAX_BUF_SIZE) {
			pollfd[2 * i + 1].events |= POLLIN;
		}
		if (size1 > 0) {
			pollfd[2 * i].events |= POLLOUT;
		}
	}

	int result;
	if ((result = poll(pollfd, 2 * opened + 2, -1)) == -1) {
		return -1;
	}

	//Checking server descriptors
	if (opened < MAX_CLIENTS && pollfd[server].revents & POLLIN) {
		if ((result = accept(pollfd[server].fd, NULL, NULL)) == -1) {
			fprintf(stderr, "Couldn't accept on file descriptor %d\n", pollfd[server].fd);
			return -1;
		} else {
			return result;
		}
	}

	//Checking client descriptors
	for (int i = 1; i < opened + 1; ++i) {
		if ((pollfd[2 * i].revents | pollfd[2 * i + 1].revents) & POLLHUP) {
			return -2 * i;
		}
		int e = 0;
		e += checkin(2 * i, 2 * i - 2);
		e += checkin(2 * i + 1, 2 * i - 1);
		e += checkout(2 * i, 2 * i - 1);
		e += checkout(2 * i + 1, 2 * i - 2);
		if (e < 0) {
			return -2 * i;
		}
	}

	//No new or disconnected clients
	return 0;
}

void close_client(int client) {
	if (client <= 0) {
		return;
	}

	//Freeing data
	buf_free(buffs[client - 2]);
	buf_free(buffs[client - 1]);
	close(pollfd[client].fd);
	close(pollfd[client + 1].fd);

	printf("Clients with file descriptors %d and %d were disconnected\n", pollfd[client].fd, pollfd[client + 1].fd);
	
	//The last client duo comes to here
	buffs[client - 2] = buffs[2 * opened - 2];
	buffs[client - 1] = buffs[2 * opened - 1];
	pollfd[client].fd = pollfd[2 * opened].fd;
	pollfd[client + 1].fd = pollfd[2 * opened + 1].fd;

	--opened;
}

int open_client(int fd1, int fd2) {
	if ((buffs[2 * opened] = buf_new(MAX_BUF_SIZE)) == NULL) {
		return -1;
	}
	if ((buffs[2 * opened + 1] = buf_new(MAX_BUF_SIZE)) == NULL) {
		return -1;
	}
	pollfd[2 * opened + 2].fd = fd1;
	pollfd[2 * opened + 3].fd = fd2;

	++opened;
	return 0;
}


void close_and_exit(int signum) {
	for (int i = 0; i < 2 * opened; ++i) {
		buf_free(buffs[i]);
	}
	for (int i = 0; i < 2 * opened + 2; ++i) {
		close(pollfd[i].fd);
	}

	if (errno == EINTR || signum == SIGINT) {
		exit(0);
	} else {
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char** argv) {
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <port1> <port2>", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct sigaction sa;
	sa.sa_handler = close_and_exit;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);

	for (int i = 0; i < 2; ++i) {
		if ((pollfd[i].fd = create_server(argv[i + 1])) == -1) {
			if (i == 1) {
				close(pollfd[0].fd);
			}

			if (errno == EINTR) {
				exit(0);
			} else {
				exit(EXIT_FAILURE);
			}
		}
	}

	while (1) {
		int result1, result2;

		do {
			if ((result1 = wait_for(0)) == -1) {
				close_and_exit(0);
			}
			close_client(-result1);
		} while (result1 <= 0);

		do {
			if ((result2 = wait_for(1)) == -1) {
				close_and_exit(0);
			}
			close_client(-result2);
		} while (result2 <= 0);

		printf("Clients with file descriptors %d and %d were connected\n", result1, result2);

		if (open_client(result1, result2) != 0) {
			if (errno == EINTR) {
				break;
			} else {
				exit(EXIT_FAILURE);
			}
		}
	}

	return 0;
}
