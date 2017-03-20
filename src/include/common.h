#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include <stdio.h>

#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - offsetof(type, member))

#define LOG_SETUP \
    printf("performing setup: %s\n", __func__);

void print_errno();
int write_to_file(const char *path, const char *buff);
int execute_bin_relative(const char *relative_path, const char *args);

#endif // __COMMON_H__
