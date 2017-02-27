#ifndef __START_OPTIONS_H__
#define __START_OPTIONS_H__

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

void get_start_options(struct start_opts *, int argc, char *argv[]);

#endif // __START_OPTIONS_H__
