#define _GNU_SOURCE

#include <init.h>
#include <common.h>
#include <uts.h>
#include <pid_ns.h>
#include <mount_ns.h>
#include <user_ns.h>
#include <list.h>

#include <sys/types.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>

static const char *PROCEED = "proceed";
static const char *FAIL = "fail";
static const size_t CHILD_STACK_SIZE = 8 * 1024 * 1024; // 8MB
static const int CLONE_FLAGS = SIGCHLD
        | CLONE_NEWIPC
        | CLONE_NEWNS
        | CLONE_NEWPID
        | CLONE_NEWUTS
        | CLONE_NEWUSER
        | CLONE_NEWNET;

static
int get_next_msg(int fd, char *buff, int len)
{
    if (read(fd, buff, len) < 0)
        goto err;

    return 0;

err:
    print_errno();
    return -1;
}

static
void daemonize(void)
{
    LOG_SETUP;

    pid_t sid;
    int fd;

    /*
     * Don't need to do fork() here as
     * we already made clone() in aucont_start
     */

    sid = setsid();
    if (sid < 0)
        exit(EXIT_FAILURE);

    if (chdir("/") < 0)
        exit(EXIT_FAILURE);

    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

        if (fd > 2)
            close (fd);
    }

    /* resettign File Creation Mask */
    umask(027);
}

static
int wait_to_proceed(int fd)
{
    char buff[20];

    if (get_next_msg(fd, buff, sizeof buff) < 0)
        return -1;

    if (!strcmp(PROCEED, buff))
        return 0;
    else {
        pr_err("While waiting to proceed expected '%s' but found '%s'.\nAboring.\n", PROCEED, buff);
        return -1;
    }
}

static
int init(void *arg)
{
    LOG_SETUP;

    char rootfs_path[100];
    struct init_info *info = (struct init_info *)arg;

    if (get_next_msg(info->pipe_fds[0], rootfs_path, sizeof rootfs_path) < 0) {
        pr_err("%s", "Error while expecting image dir path\n");
        exit(EXIT_FAILURE);
    }
    info->rootfs_path = rootfs_path;

    char pid_str[10];
    if (get_next_msg(info->pipe_fds[0], pid_str, sizeof pid_str) < 0) {
        pr_warn("%s", "Could not receive pid in root pid ns\n");
    }
    info->pid = atoi(pid_str);

    if (wait_to_proceed(info->pipe_fds[0]) < 0) {
        pr_err("%s", "Can not proceed, aborting\n");
        exit(EXIT_FAILURE);
    }

    setuid(0);
    seteuid(0);
    setgid(0);
    setegid(0);

    if (setup_hostname() < 0) {
        pr_err("%s", "Could not setup new host name\n");
        exit(EXIT_FAILURE);
    }

    if (mount_rootfs(info->rootfs_path) < 0) {
        pr_err("%s", "Could not mount new rootfs\n");
        exit(EXIT_FAILURE);
    }

    pr_success("%s", "All setup done. Executing user command\n");

    const char *cmd = info->opts->cmd;
    const char *args[list_length(info->opts->cmd_args) + 2];
    args[0] = cmd;

    int i = 1;
    struct list_head *pos;
    list_for_each(pos, info->opts->cmd_args)
        args[i++] = ((struct cmd_arg *)container_of(pos, struct cmd_arg, args))->arg;
    args[i] = NULL;

    printf("%d\n", info->pid);
    fflush(stdout);
    if (info->opts->detached)
        daemonize();

    if (execve(cmd, (char * const*)args, NULL) < 0) {
        pr_err("%s", "Could not execute user's command\n");
        print_errno();
        exit(EXIT_FAILURE);
    }

    return 0;
}

int clone_container_init(struct init_info *info)
{
    if (pipe2(info->pipe_fds, O_CLOEXEC | O_DIRECT) < 0)
        goto err;
    void *stack = malloc(CHILD_STACK_SIZE);
    stack = (void *)((char *)stack + CHILD_STACK_SIZE);
    if (stack == NULL)
        goto err;
    if ((info->pid = clone(init, stack, CLONE_FLAGS, info)) < 0) {
        free(stack);
        goto err;
    }
    if (close(info->pipe_fds[0]) < 0) {
        free(stack);
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}

int send_init(int fd, const char *msg)
{
    int ret = write(fd, msg, strlen(msg));
    if (ret < 0)
        print_errno();
    return ret;
}

int notify_init_proceed(int fd)
{
    return send_init(fd, PROCEED);
}

int notify_init_fail(int fd)
{
    return send_init(fd, FAIL);
}

void kill_init(int pid)
{
    if (kill(pid, SIGKILL) < 0)
        print_errno();
}
