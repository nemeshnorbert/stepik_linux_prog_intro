#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

typedef int (*func_ptr_t)(int);

bool init_library(const char* lib_path, const char* func_name, func_ptr_t* ppfunc) {
    void* lib_handle = dlopen(lib_path, RTLD_LAZY);
    if (lib_handle == NULL) {
        *ppfunc = NULL;
        return false;
    }
    *ppfunc = (func_ptr_t)dlsym(lib_handle, func_name);
    if (*ppfunc == NULL) {
        *ppfunc = NULL;
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Please enter shared library path, function name and function argument\n");
        return 1;
    }
    const char* lib_path = argv[1];
    const char* func_name = argv[2];
    const char* arg_str = argv[3];
    func_ptr_t pfunc;
    if (init_library(lib_path, func_name, &pfunc)) {
        size_t arg_len = strlen(arg_str);
        char* arg_end = NULL;
        int arg = strtol(arg_str, &arg_end, 10);
        if (arg_end != NULL) {
            printf("%d\n", pfunc(arg));
            return 0;
        } else {
            printf("Cannot parse function argument\n");
        }
    } else {
        printf("Cannot load function %s from %s library\n", argv[2], argv[1]);
    }
    return 1;
}
