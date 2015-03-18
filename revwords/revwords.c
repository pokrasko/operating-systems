#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <helpers.h>

int main()
{
    int count, i;
    char bufin[4097], bufout[4097];

    count = read_until(STDIN_FILENO, bufin, sizeof(bufin), ' ');
    while (count > 0) {
        if (count == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return 1;
        }
        int size = count - 1;
        if (bufin[size] != ' ') {
            ++size;
        } else {
            bufout[size] = ' ';
        }
        for (i = 0; i < size; ++i) {
            bufout[i] = bufin[size - 1 - i];
        }
        write_(STDOUT_FILENO, bufout, count);
        count = read_until(STDIN_FILENO, bufin, sizeof(bufin), ' ');
    }

    return 0;
}
