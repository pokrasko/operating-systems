#define _GNU_SOURCE
#define _POSIX_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>

#include <netinet/in.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

void error(const char* msg);
ssize_t read_(int fd, void* buf, size_t count);
int read__(int fd, void* buf, size_t count1, size_t count2);
ssize_t read_until(int fd, void* buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv[]);
void thiserror();
ssize_t write_(int fd, const void* buf, size_t count);
int write__(int fd, const void* buf, size_t count1, size_t count2);

struct execargs_t {
	int size;
	char** words;
};

typedef struct execargs_t execargs_t;

int exec(execargs_t* args);
int runpiped(execargs_t** programs, size_t n);
