#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>


int counter1 = 0;
int counter2 = 0;

void sigusr1_handler(int signalno) {
    ++counter1;
}

void sigusr2_handler(int signalno) {
    ++counter2;
}

void sigterm_handler(int signalno) {
    printf("%d %d\n", counter1, counter2);
    exit(EXIT_SUCCESS);
}

int main() {
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);
    signal(SIGTERM, sigterm_handler);
    while (1) {
        usleep(100000);
    }
    exit(EXIT_SUCCESS);
}
