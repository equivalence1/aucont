#ifndef __START_OPTIONS_H__
#define __START_OPTOONS_H__

#include "list.h"

struct start_opts {
    int cpu;
    int detached;
    int verbose;
    const char *net;
    const char *image_path;
    const char *cmd;
    struct list_head *cmd_args;
};

struct cmd_arg {
    const char *arg;
    struct list_head args;
};

void get_start_opts(struct start_opts *, int, char **);

#endif // __START_OPTIONS_H__
