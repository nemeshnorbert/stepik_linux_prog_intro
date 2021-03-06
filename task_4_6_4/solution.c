#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void sigurg_handler(int signalno) {
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    pid_t pid = getpid();
    printf("%d\n", pid);
    signal(SIGURG, sigurg_handler);
    if (fclose(stdout) != 0) {
        perror("Cannot close stdout");
        exit(EXIT_FAILURE);
    }
    if (daemon(0, 0) == -1) {
        char error_message[128];
        sprintf(error_message, "Cannot demonize process %d", pid);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    pause();
    exit(EXIT_SUCCESS);
}

