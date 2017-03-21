#define _GNU_SOURCE

#include <common.h>

#include <sched.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>

static
int join_ns(int pid, const char *ns_name)
{
    char ns[50];
    snprintf(ns, sizeof ns, "/proc/%d/ns/%s", pid, ns_name);

    int fd = open(ns, O_RDONLY);
    if (fd < 0) {
        pr_err("Could not open %s\n", ns);
        goto err;
    }
    if (setns(fd, 0) < 0) {
        pr_err("Could not setns %s\n", ns);
        goto err;
    }
    close(fd);

    return 0;

err:
    print_errno();
    return -1;
}


static
int go_to_process_ns(int pid)
{
    if (join_ns(pid, "net") < 0)
        return -1;
    if (join_ns(pid, "ipc") < 0)
        return -1;
    if (join_ns(pid, "user") < 0)
        return -1;
    if (join_ns(pid, "pid") < 0)
        return -1;
    if (join_ns(pid, "uts") < 0)
        return -1;
    if (join_ns(pid, "mnt") < 0)
        return -1;
    return 0;
}

int main(int argc, char *argv[])
{
    setuid(0);

    int pid = atoi(argv[1]);
    char *cmd = argv[2];

    if (go_to_process_ns(pid) < 0)
        exit(EXIT_FAILURE);

    char *args[argc - 1];
    for (int i = 0; i < argc - 1; i++) {
        args[i] = argv[i + 2];
    }
    args[argc - 2] = NULL;

    pid = fork();
    if (pid < 0) {
        pr_err("%s", "Could not fork\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (execve(cmd, args, NULL) < 0) {
            pr_err("%s", "Could not exec\n");
            print_errno();
            exit(EXIT_FAILURE);
        }
    }
}
