#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


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

long parse_port(const char* text) {
    long port = 0;
    if (!parse_long(text, &port, 10)) {
        char error_message[128];
        sprintf(error_message, "Cannot parse port %s", text);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    return port;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage ./solution <port>\n");
        exit(EXIT_FAILURE);
    }
    int port = (int)parse_port(argv[1]);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in local = {0};
    inet_aton("127.0.0.1", &local.sin_addr);
    local.sin_port = htons(port);
    local.sin_family = AF_INET;

    int status = bind(sock, (struct sockaddr*)&local, sizeof(local));
    if (status == -1) {
        char error_message[128];
        sprintf(error_message, "Cannot bind socket %d", sock);
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    char buffer[5 * 1024];
    bool finish_listening = false;
    while (true) {
        int bytes_count = read(sock, buffer, sizeof(buffer));
        if (bytes_count == -1) {
            char error_message[128];
            sprintf(error_message, "Cannot read from socket %d", sock);
            perror(error_message);
            exit(EXIT_FAILURE);
        }
        buffer[bytes_count] = '\0';
        if (strcmp(buffer, "OFF\n") == 0) {
            break;
        }
        printf("%s\n", buffer);
    }

    if (close(sock) == -1) {
        char error_message[128];
        sprintf(error_message, "Cannot close socket %d", sock);
        perror(error_message);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

