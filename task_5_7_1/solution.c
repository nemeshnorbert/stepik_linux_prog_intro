#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>



int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage ./solution <site-addr>\n");
        exit(EXIT_FAILURE);
    }

    struct hostent* host_info = gethostbyname(argv[1]);

    if (host_info == NULL) {
        char error_message[128];
        sprintf(error_message, "Cannot get information for the host %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    size_t idx = 0;
    while (host_info->h_addr_list[idx] != NULL) {
        struct in_addr* inaddr = (struct in_addr*)(host_info->h_addr_list[idx]);
        printf("%s\n", inet_ntoa(*inaddr));
        ++idx;
    }

    exit(EXIT_SUCCESS);
}

