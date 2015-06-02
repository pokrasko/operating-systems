#define LIMIT 4096

#include <netdb.h>
#include <signal.h>

#include <bufio.h>

int main(int argc, char** argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: filesender <port> <file>");
	}

	struct addrinfo hints;
	struct addrinfo* result;
	struct sockaddr_in c;
	struct buf_t* buf;
	int f, fd, s;
	int a;
	int res, fail = 0;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	if ((res = getaddrinfo(NULL, argv[1], &hints, &result)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	for (struct addrinfo* p = result; p != NULL; p = p->ai_next) {
		if (socket(p->ai_family, p->ai_socktype, p->ai_protocol) == -1) {
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

	if ((fd = accept(s, (struct sockaddr*) &c, sizeof(c))) == -1) {
		fprintf(stderr, "Could not accept\n");
		fail = 1;
		goto accept_fail;
	}

	if ((f = open(argv[2], O_RDONLY)) == -1) {
		fprintf(stderr, "Could not open a file\n");
		fail = 1;
		goto file_fail;
	}

	if ((buf = buf_new(LIMIT)) == NULL) {
		fprintf(stderr, "Could not create a buffer\n");
		fail = 1;
		goto buf_fail;
	}

	while ((a = buf_fill(f, buf, LIMIT)) != 0) {
		if (a == -1) {
			fprintf(stderr, "Could not fill a buffer\n");
			fail = 1;
			goto fill_fail;
		}
		while (buf_size(buf) != 0) {
			if (buf_flush(fd, buf, buf_size(buf)) != -1) {
				fprintf(stderr, "Could not flush a buffer\n");
				fail = 1;
				goto flush_fail;
			}
		}
	}

flush_fail:
fill_fail:
	buf_free(buf);
buf_fail:
	close(f);
file_fail:
	close(fd);
accept_fail:
listen_fail:
	close(s);
	if (fail) {
		exit(EXIT_FAILURE);
	}

	return 0;
}
