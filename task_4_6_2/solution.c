#include <fcntl.h>
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

bool parse_long(const char* text, long* out, int base) {
    char *end;
    if (text[0] == '\0' || isspace(text[0]))
        return false;
    long value = strtol(text, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (value > INT_MAX || (errno == ERANGE && value == LONG_MAX))
        return false;
    if (value < INT_MIN || (errno == ERANGE && value == LONG_MIN))
        return false;
    *out = value;
    return true;
}

int main(int argc, char* argv) {
    int fds[2];
    long totals[2] = {0, 0};
    fd_set watchset;
    fd_set inset;
    if ((fds[0] = open("in1", O_RDONLY)) < 0) {
        perror("Cannot open pipe in1");
        exit(EXIT_FAILURE);
    }
    if ((fds[1] = open("in2", O_RDONLY)) < 0) {
        perror("Cannot open pipe in2");
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&watchset);
    FD_SET(fds[0], &watchset);
    FD_SET(fds[1], &watchset);
    int maxfd = max(fds[0], fds[1]);
    char buffer[2];
    while (FD_ISSET(fds[0], &watchset) || FD_ISSET(fds[1], &watchset)) {
        inset = watchset;
        if (select(maxfd + 1, &inset, NULL, NULL, NULL) < 0) {
            break;
            perror("Select is not working");
            exit(EXIT_FAILURE);
        }
        int idx = 0;
        for (idx = 0; idx < 2; ++idx) {
            if (FD_ISSET(fds[idx], &inset)) {
                int count = read(fds[idx], buffer, sizeof(buffer) - 1);
                if (count < 0) {
                    char error_message[64];
                    sprintf(error_message, "Failed to read from pipe (fd=%d)", fds[idx]);
                    perror(error_message);
                    exit(EXIT_FAILURE);
                } else if (count == 0) {
                    if (close(fds[idx]) == -1) {
                        char error_message[64];
                        sprintf(error_message, "Failed to close pipe (fd=%d)", fds[idx]);
                        perror(error_message);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    buffer[count] = '\0';
                    long value;
                    if (parse_long(buffer, &value, 10)) {
                        totals[idx] += value;
                    } else {
                        char error_message[64];
                        sprintf(error_message, "Cannot parse input from the pipe (fd=%d)", fds[idx]);
                        perror(error_message);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
    printf("%ld\n", totals[0] +  totals[1]);
    exit(EXIT_SUCCESS);
}
