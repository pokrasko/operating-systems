#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

void error(const char* msg);
ssize_t read_(int fd, void* buf, size_t count);
int read__(int fd, void* buf, size_t count);
ssize_t read_until(int fd, void* buf, size_t count, char delimiter);
int spawn(const char* file, char* const argv[]);
void thiserror();
ssize_t write_(int fd, const void* buf, size_t count);
int write__(int fd, const void* buf, size_t count);

struct execargs_t {
	int size;
	char** words;
};

typedef struct execargs_t execargs_t;

int exec(execargs_t* args);
int runpiped(execargs_t** programs, size_t n);
