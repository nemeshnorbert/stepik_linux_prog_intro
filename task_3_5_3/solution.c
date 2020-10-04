#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define MAX_COMM_SIZE 256

struct proc_stat {
    int pid;                                    // 1  %d   int
    char comm[MAX_COMM_SIZE];                   // 2  %s   char*
    char state;                                 // 3  %c   char
    int ppid;                                   // 4  %d   int
    int pgrp;                                   // 5  %d   int
    int session;                                // 6  %d   int
    int tty_nr;                                 // 7  %d   int
    int tpgid;                                  // 8  %d   int
    unsigned int flags;                         // 9  %u   unsigned int
    unsigned long int minflt;                   // 10 %lu  unsigned long int
    unsigned long int cminflt;                  // 11 %lu  unsigned long int
    unsigned long int majflt;                   // 12 %lu  unsigned long int
    unsigned long int cmajflt;                  // 13 %lu  unsigned long int
    unsigned long int utime;                    // 14 %lu  unsigned long int
    unsigned long int stime;                    // 15 %lu  unsigned long int
    long cutime;                                // 16 %ld  long
    long cstime;                                // 17 %ld  long
    long priority;                              // 18 %ld  long
    long nice;                                  // 19 %ld  long
    long num_threads;                           // 20 %ld  long
    long itrealvalue;                           // 21 %ld  long
    unsigned long long starttime;               // 22 %llu unsigned long long
    unsigned long int vsize;                    // 23 %lu  unsigned long int
    long int rss;                               // 24 %ld  long
    unsigned long int rsslim;                   // 25 %lu  unsigned long int
    unsigned long int startcode;                // 26 %lu  unsigned long int
    unsigned long int endcode;                  // 27 %lu  unsigned long int
    unsigned long int startstack;               // 28 %lu  unsigned long int
    unsigned long int kstkesp;                  // 29 %lu  unsigned long int
    unsigned long int kstkeip;                  // 30 %lu  unsigned long int
    unsigned long int signal;                   // 31 %lu  unsigned long int
    unsigned long int blocked;                  // 32 %lu  unsigned long int
    unsigned long int sigignore;                // 33 %lu  unsigned long int
    unsigned long int sigcatch;                 // 34 %lu  unsigned long int
    unsigned long int wchan;                    // 35 %lu  unsigned long int
    unsigned long int nswap;                    // 36 %lu  unsigned long int
    unsigned long int cnswap;                   // 37 %lu  unsigned long int
    int exit_signal;                            // 38 %d   int
    int processor;                              // 39 %d   int
    unsigned int rt_priority;                   // 40 %u   unsigned int
    unsigned int policy;                        // 41 %u   unsigned int
    unsigned long long delayacct_blkio_ticks;   // 42 %llu unsigned long long
    unsigned long int guest_time;               // 43 %lu  unsigned long int
    long cguest_time;                           // 44 %ld  long
    unsigned long int start_data;               // 45 %lu  unsigned long int
    unsigned long int end_data;                 // 46 %lu  unsigned long int
    unsigned long int start_brk;                // 47 %lu  unsigned long int
    unsigned long int arg_start;                // 48 %lu  unsigned long int
    unsigned long int arg_end;                  // 49 %lu  unsigned long int
    unsigned long int env_start;                // 50 %lu  unsigned long int
    unsigned long int env_end;                  // 51 %lu  unsigned long int
    int exit_code;                              // 52 %d   int

};

void read_proc_stat_impl(FILE* fid, struct proc_stat* stat) {
    assert(fid != NULL);

    char parenthsis;

    fscanf(fid, "%d", &(stat->pid));
    fscanf(fid, " %c", &parenthsis);
    assert(parenthsis == '(');
    fscanf(fid, "%[^)]s", stat->comm);
    fscanf(fid, "%c", &parenthsis);
    assert(parenthsis == ')');

    const char fmt[] = (
        " %c %d %d "
        "%d %d %d %u %lu "
        "%lu %lu %lu %lu "
        "%lu %ld %ld %ld "
        "%ld %ld %ld %llu "
        "%lu %ld %lu %lu "
        "%lu %lu %lu %lu "
        "%lu %lu %lu %lu "
        "%lu %lu %lu %d "
        "%d %u %u %llu %lu "
        "%ld %lu %lu %lu %lu "
        "%lu %lu %lu %d");

    fscanf(fid, fmt,
        &(stat->state),
        &(stat->ppid),
        &(stat->pgrp),
        &(stat->session),
        &(stat->tty_nr),
        &(stat->tpgid),
        &(stat->flags),
        &(stat->minflt),
        &(stat->cminflt),
        &(stat->majflt),
        &(stat->cmajflt),
        &(stat->utime),
        &(stat->stime),
        &(stat->cutime),
        &(stat->cstime),
        &(stat->priority),
        &(stat->nice),
        &(stat->num_threads),
        &(stat->itrealvalue),
        &(stat->starttime),
        &(stat->vsize),
        &(stat->rss),
        &(stat->rsslim),
        &(stat->startcode),
        &(stat->endcode),
        &(stat->startstack),
        &(stat->kstkesp),
        &(stat->kstkeip),
        &(stat->signal),
        &(stat->blocked),
        &(stat->sigignore),
        &(stat->sigcatch),
        &(stat->wchan),
        &(stat->nswap),
        &(stat->cnswap),
        &(stat->exit_signal),
        &(stat->processor),
        &(stat->rt_priority),
        &(stat->policy),
        &(stat->delayacct_blkio_ticks),
        &(stat->guest_time),
        &(stat->cguest_time),
        &(stat->start_data),
        &(stat->end_data),
        &(stat->start_brk),
        &(stat->arg_start),
        &(stat->arg_end),
        &(stat->env_start),
        &(stat->env_end),
        &(stat->exit_code));
}

bool read_proc_stat(pid_t pid, struct proc_stat* stat) {
    char stat_path[128];
    sprintf(stat_path, "/proc/%d/stat", pid);
    FILE* fid = fopen(stat_path, "r");
    if (fid == NULL) {
        char error_message[1024];
        sprintf(error_message, "Cannot open %s", stat_path);
        perror(error_message);
        return false;
    }
    read_proc_stat_impl(fid, stat);
    if (fclose(fid)) {
        char error_message[1024];
        sprintf(error_message, "Cannot close %s", stat_path);
        perror(error_message);
        return false;
    }
    return true;
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

int is_proc_dir(const struct dirent* entry) {
    if (entry->d_type == DT_DIR) {
        pid_t pid;
        if (parse_pid(entry->d_name, &pid)) {
            return 1;
        }
        return 0;
    }
    return 0;
}

bool count_proc(const char* proc_name, int* count) {
    assert(proc_name != NULL);
    struct dirent **entries;
    int entries_count = scandir("/proc", &entries, is_proc_dir, alphasort);
    if (entries_count < 0) {
        perror("Error while reading directory /proc: ");
        return false;
    }
    *count = 0;
    int idx;
    for (idx = 0; idx < entries_count; ++idx) {
        pid_t pid;
        if (parse_pid(entries[idx]->d_name, &pid)) {
            struct proc_stat stat;
            if (read_proc_stat(pid, &stat)) {
                if (strcmp(stat.comm, proc_name) == 0) {
                    ++(*count);
                }
            } else {
                exit(EXIT_FAILURE);
            }
        } else {
            exit(EXIT_FAILURE);
        }
        free(entries[idx]);
    }
    free(entries);
    return true;
}

int main(int argc, char* argv[]) {
    const char proc_name[] = "genenv";
    int count = 0;
    if (count_proc(proc_name, &count)) {
        printf("%d\n", count);
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_FAILURE);
}
