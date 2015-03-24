#include <helpers.h>

int main()
{
    char buf[4096];
    size_t count;

    do {
        count = read_(STDIN_FILENO, buf, sizeof(buf));
        if (count == -1) {
            error(strerror(errno));
        }
        size_t written = write_(STDOUT_FILENO, buf, count);
        if (written < count) {
            error("Unexpected output error");
        }
    } while (count > 0);

    return 0;
}
