#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/shm.h>
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

key_t parse_shmem_key(const char* text) {
    key_t key;
    long value;
    if (parse_long(text, &value, 10)) {
        return (key_t)(value);
    }
    exit(EXIT_FAILURE);
}

int get_shmid(key_t key, size_t size, int flags) {
    int shmid;
    if ((shmid = shmget(key, size, flags)) < 0) {
        char error_message[128];
        sprintf(error_message, "Cannot get shared memory id for key %d", key);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    return shmid;
}

int create_shmid(key_t key, size_t size, int flags) {
    int shmid;
    if ((shmid = shmget(key, size, flags)) < 0) {
        char error_message[128];
        sprintf(error_message, "Cannot get address of shared memory with id %d", shmid);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    return shmid;
}

void* attach_shm(int shmid, int flags) {
    void* shm;
    if ((shm = shmat(shmid, NULL, flags)) == (void*)(-1)) {
        char error_message[128];
        sprintf(error_message, "Cannot get address of shared memory with id %d", shmid);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    return shm;
}

void detach_shm(void* shm) {
    if (shmdt(shm) == -1) {
        char error_message[128];
        sprintf(error_message, "Cannot detach from shared memory with address %p", shm);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
}

void do_summation(void* shm1, void* shm2, void* shm3, size_t values_count) {
    int* in1 = (int*)(shm1);
    int* in2 = (int*)(shm2);
    int* out = (int*)(shm3);
    size_t idx;
    for (idx = 0; idx < values_count; ++idx) {
        int* here = (int*)(out + idx);
        *here = *(in1 + idx) + *(in2 + idx);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage ./solution <shmem-key1> <shmem-key2>\n");
        exit(EXIT_FAILURE);
    }

    key_t key1 = parse_shmem_key(argv[1]);
    key_t key2 = parse_shmem_key(argv[2]);
    key_t key3 = 123456;

    const size_t IN_SHMEM_SIZE = 1000;
    const size_t OUT_SHMEM_SIZE = 1000;

    int shmid1 = get_shmid(key1, IN_SHMEM_SIZE, 0600);
    int shmid2 = get_shmid(key2, IN_SHMEM_SIZE, 0600);
    int shmid3 = create_shmid(key3, OUT_SHMEM_SIZE, IPC_CREAT | 0666);

    void* shm1 = attach_shm(shmid1, SHM_RDONLY);
    void* shm2 = attach_shm(shmid2, SHM_RDONLY);
    void* shm3 = attach_shm(shmid3, 0);

    const size_t VALUES_COUNT = 100;
    do_summation(shm1, shm2, shm3, VALUES_COUNT);

    detach_shm(shm1);
    detach_shm(shm2);
    detach_shm(shm3);

    printf("%d\n", key3);

    exit(EXIT_SUCCESS);
}

