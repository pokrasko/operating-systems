#define LIMIT 4096

#include <bufio.h>


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
		fprintf(stderr, "Could not bind on port %s\n", port);
		return -1;
	}

	freeaddrinfo(result);

	if (listen(s, 1) == -1) {
		close(s);
		fprintf(stderr, "Could not listen on port %s\n", port);
		return -1;
	}

	printf("Socket %d on port %s is opened\n", s, port);
	return s;
}


int create_client(int sfd) {
	struct sockaddr_in c;
	socklen_t size;
	int fd;
	
	if ((fd = accept(sfd, (struct sockaddr*) &c, &size)) == -1) {
		fprintf(stderr, "Could not accept\n");
		return -1;
	}

	return fd;
}


void run_client(int in, int out) {

	struct buf_t* buf;
	if ((buf = buf_new(LIMIT)) == NULL) {
		fprintf(stderr, "Could not create a buffer\n");
		return;
	}

	int result, result2;
	while ((result = buf_fill_at_once(in, buf, LIMIT)) != 0) {
		if (result == -1) {
			fprintf(stderr, "Could not fill a buffer\n");
			buf_free(buf);
			return;
		}
		
		if ((result2 = buf_flush(out, buf, buf_size(buf))) == -1) {
			fprintf(stderr, "Could not flush a buffer\n");
			buf_free(buf);
			return;
		}
	}

	buf_free(buf);

	return;
}


int main(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <port1> <port2>", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd[2], s[2];

	for (int i = 0; i < 2; ++i) {
		if ((s[i] = create_server(argv[i + 1])) == -1) {
			if (i == 1) {
				close(s[0]);
			}
			exit(EXIT_FAILURE);
		}
	}

	while (1) {
		for (int i = 0; i < 2; ++i) {
			if ((fd[i] = create_client(s[i])) == -1) {
				if (i == 1) {
					close(fd[0]);
				}
				close(s[1]);
				close(s[0]);
				exit(EXIT_FAILURE);
			}
		}

		for (int i = 0; i < 2; ++i) {
			if (fork() == 0) {
				printf("Child %d was born\n", i + 1);
				run_client(fd[i], fd[1 - i]);
				printf("Child %d died\n", i + 1);
				exit(0);
			}
		}
		
		close(fd[1]);
		close(fd[0]);
	}

    return 0;
}
