#define _GNU_SOURCE

#include <start_options.h>
#include <init.h>
#include <user_ns.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

struct start_opts opts;
struct init_info init;

int main(int argc, char *argv[])
{
    get_start_options(&opts, argc, argv);
    init.opts = &opts;

    int ret = clone_container_init(&init);
    if (ret < 0)
        exit(EXIT_FAILURE);

    ret = setup_user_mappings(&init);
    if (ret < 0)
        exit(EXIT_FAILURE);

    ret = notify_init_proceed(init.pipe_fds[1]);
    if (ret < 0) {
        printf("Could not notify init to proceed.\nKilling init and aborting.\n");
        kill_init(init.pid);
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(init.pid, &status, 0);
}
