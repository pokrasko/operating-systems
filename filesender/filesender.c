#define LIMIT 4096

#include <bufio.h>

int main(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: filesender <port> <file>");
		exit(EXIT_FAILURE);
	}

	struct addrinfo hints;
	struct addrinfo *p, *result;
	struct sockaddr_in c;
	socklen_t size = sizeof(c);
	pid_t pid;
	int f, fd, s;
	int res, fail = 0;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, argv[1], &hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
		exit(EXIT_FAILURE);
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
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);

	if (listen(s, 1) == -1) {
		fprintf(stderr, "Could not listen\n");
		fail = 1;
		goto listen_fail;
	}

	while (1) {
		if ((fd = accept(s, (struct sockaddr*) &c, &size)) == -1) {
			fprintf(stderr, "Could not accept\n");
			fail = 1;
			goto accept_fail;
		}

		if ((pid = fork()) == 0) {
			printf("Child was born\n");
			if ((f = open(argv[2], O_RDONLY)) == -1) {
				fprintf(stderr, "Could not open a file\n");
				exit(0);
			}

			struct buf_t* buf;
			if ((buf = buf_new(LIMIT)) == NULL) {
				fprintf(stderr, "Could not create a buffer\n");
				close(f);
				exit(0);
			}
	
			int a;
			while ((a = buf_fill(f, buf, LIMIT)) != 0) {
				if (a == -1) {
					fprintf(stderr, "Could not fill a buffer\n");
					buf_free(buf);
					close(f);
					exit(0);
				}
				while (buf_size(buf) != 0) {
					if (buf_flush(fd, buf, buf_size(buf)) == -1) {
						fprintf(stderr, "Could not flush a buffer\n");
						buf_free(buf);
						close(f);
						exit(0);
					}
				}
			}

			buf_free(buf);
			close(f);
			printf("Child died\n");
			exit(0);
		}
		
		close(fd);
	}

accept_fail:
listen_fail:
	close(s);
	if (fail) {
		exit(EXIT_FAILURE);
	}

    return 0;
}
