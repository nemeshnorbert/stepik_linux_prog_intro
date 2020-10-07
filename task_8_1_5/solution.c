/* #include <unistd.h> */
#include <stdio.h>
/* #include <stdlib.h> */
#include <getopt.h>
/* #include <string.h> */

    /* -q | --query (опциональный, но обязательно требует аргумента) */
    /* -i | --longinformationrequest (опциональный) */
    /* -v | --version (опциональный) */



int main(int argc, char* argv[]) {
    int opindex = 0;
    struct option opts[] = {
        {"query", required_argument, 0, 'q'},
        {"longinformationrequest", no_argument, 0, 'i'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    char opchar;
    while (-1 != (opchar = getopt_long(argc, argv, "q:iv", opts, &opindex))) {
        switch (opchar) {
            case 'q':
                break;
            case 'i':
                break;
            case 'v':
                break;
            default:
                printf("-\n");
                return 0;
        }
    }
    printf("+\n");
    return 0;
}
