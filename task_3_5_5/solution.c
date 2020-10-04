#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>


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
    if (*end != '\0')
        return false;
    *out = value;
    return true;
}

bool parse_pid(const char* text, pid_t* pid) {
    long value = 0;
    if (parse_long(text, &value, 10)) {
        *pid = (pid_t)value;
        return true;
    }
    return false;
}

bool path_exists(const char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) == -1) {
        return false;
    }
    return true;
}

bool process_exists(pid_t pid) {
    char path[128];
    sprintf(path, "/proc/%d", pid);
    return path_exists(path);
}

struct pids_array {
    pid_t* pids;
    int size;
    int capacity;
};

void init_pids_array(struct pids_array* pids_arr, int capacity) {
    assert(pids_arr != NULL);
    assert(capacity > 0);
    pids_arr->capacity = capacity;
    pids_arr->size = 0;
    pids_arr->pids = (pid_t*)malloc(pids_arr->capacity * sizeof(pid_t));
    if (pids_arr->pids == NULL) {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }
}

void release_pids_array(struct pids_array* pids_arr) {
    assert(pids_arr != NULL);
    free(pids_arr->pids);
    pids_arr->capacity = 0;
    pids_arr->size = 0;
}

int get_pids_array_size(struct pids_array* pids_arr) {
    assert(pids_arr != NULL);
    return pids_arr->size;
}

int get_pids_array_capacity(struct pids_array* pids_arr) {
    assert(pids_arr != NULL);
    return pids_arr->capacity;
}

pid_t get_pid(struct pids_array* pids_arr, int idx) {
    assert(pids_arr != NULL);
    return pids_arr->pids[idx];
}

void swap_int(int* lhs, int* rhs) {
    int temp = *rhs;
    *rhs = *lhs;
    *lhs = temp;
}

void swap_pid_ptr(pid_t** lhs, pid_t** rhs) {
    pid_t* temp = *rhs;
    *rhs = *lhs;
    *lhs = temp;
}

void swap_pids_arrays(struct pids_array* lhs, struct pids_array* rhs) {
    swap_int(&lhs->size, &rhs->size);
    swap_int(&lhs->capacity, &rhs->capacity);
    swap_pid_ptr(&lhs->pids, &rhs->pids);
}

void add_pid_to_pids_array(struct pids_array* pids_arr, pid_t pid) {
    assert(pids_arr != NULL);
    if (pids_arr->size >= pids_arr->capacity) {
        struct pids_array new_pids_arr;
        init_pids_array(&new_pids_arr, pids_arr->capacity * 2);
        int idx;
        for (idx = 0; idx < get_pids_array_size(pids_arr); ++idx) {
            add_pid_to_pids_array(&new_pids_arr, get_pid(pids_arr, idx));
        }
        swap_pids_arrays(pids_arr, &new_pids_arr);
        release_pids_array(&new_pids_arr);
    }
    ++pids_arr->size;
    pids_arr->pids[pids_arr->size - 1] = pid;
}

void read_proc_children(const char path[128], struct pids_array* pids_arr) {
    FILE* fid = fopen(path, "r");
    if (fid == NULL) {
        char error_message[256];
        sprintf(error_message, "Cannot open file %s", path);
        perror(error_message);
        exit(EXIT_FAILURE);
    }
    pid_t pid = 0;
    while (fscanf(fid, "%d", &pid) > 0) {
        add_pid_to_pids_array(pids_arr, pid);
    }
}

void get_proc_children(pid_t pid, struct pids_array* pids_arr) {
    char path[128];
    sprintf(path, "/proc/%d/task/%d/children", pid, pid);
    read_proc_children(path, pids_arr);
}

int get_descendant_count(pid_t pid) {
    if (process_exists(pid)) {
        struct pids_array children_pids;
        init_pids_array(&children_pids, 10);
        int descendant_count = 0;
        get_proc_children(pid, &children_pids);
        int idx;
        for (idx = 0; idx < get_pids_array_size(&children_pids); ++idx) {
            descendant_count += get_descendant_count(get_pid(&children_pids, idx));
        }
        release_pids_array(&children_pids);
        return descendant_count + 1;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: solution <pid>\n");
    }
    pid_t pid = 0;
    if (!parse_pid(argv[1], &pid)) {
        printf("Cannot parse pid %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    printf("%d\n", get_descendant_count(pid));
    exit(EXIT_SUCCESS);
}
