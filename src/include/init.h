#ifndef __INIT_H__
#define __INIT_H__

#include "start_options.h"

struct init_info {
    int pid;
    int pipe_fds[2];
    struct start_opts *opts;
};

int clone_container_init(struct init_info *);
int notify_init_proceed(int);
int notify_init_fail(int);
void kill_init(int);

#endif // __INIT_H__
