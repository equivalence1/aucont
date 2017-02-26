#define _GNU_SOURCE

#include <start_options.h>
#include <init.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

struct start_opts opts;

int main(int argc, char *argv[])
{
    get_start_options(&opts, argc, argv);

    int pid = clone_container_init();
    if (pid < 0) {
        printf("%s\n", strerror(errno));
        return pid;
    }

    int status;
    waitpid(pid, &status, 0);
}
