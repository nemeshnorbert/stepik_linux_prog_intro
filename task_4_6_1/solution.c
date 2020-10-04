#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

char* build_command(const char* program_name, const char* program_arg) {
    size_t program_name_len = strlen(program_name);
    size_t program_arg_len = strlen(program_arg);
    size_t command_len = program_name_len + program_arg_len + 4;
    char* command = (char*)malloc(command_len * sizeof(char));
    if (command == NULL) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
    sprintf(command, "%s %s", program_name, program_arg);
    return command;
}

int count_zeros(FILE* pipe) {
    char buffer[1024];
    const size_t BUFFER_SIZE = 1024;
    int counter = 0;
    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
        char* ptr = buffer;
        while (*ptr != '\0') {
            if (*ptr == '0') {
                ++counter;
            }
            ++ptr;
        }
    }
    return counter;

}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: solution <program> <program-arg>\n");
        exit(EXIT_FAILURE);
    }

    char* command = build_command(argv[1], argv[2]);
    FILE* pipe = popen(command, "r");
    if (pipe == NULL) {
        perror("Cannot open pipe");
        exit(EXIT_FAILURE);
    }
    int zeros_count = count_zeros(pipe);
    printf("%d\n", zeros_count);
    if (pclose(pipe) == -1) {
        perror("Cannot close pipe");
        exit(EXIT_FAILURE);
    }
    free(command);
    exit(EXIT_SUCCESS);
}
