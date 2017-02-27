#define _GNU_SOURCE

#include <init.h>
#include <common.h>
#include <uts.h>

#include <sys/types.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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
int wait_to_proceed(int fd)
{
    char buff[20];

    if (read(fd, buff, sizeof buff) < 0)
        goto err;

    if (!strcmp(PROCEED, buff))
        return 0;
    else {
        printf("While waiting to proceed expected '%s' but found '%s'.\nAboring.\n", PROCEED, buff);
        return -1;
    }

err:
    print_errno();
    return -1;
}

static
int init(void *arg)
{
    log_setup;

    struct init_info *info = (struct init_info *)arg;

    if (setup_hostname() < 0)
        exit(EXIT_FAILURE);

    char hostname[15];
    gethostname(hostname, 15);

    if (wait_to_proceed(info->pipe_fds[0]) < 0)
        exit(EXIT_FAILURE);

    printf("my hostname is '%s', my pid is %d, my uid is %d\n", hostname, getpid(), getuid());
    execl("/bin/sh", "/bin/sh", NULL);

    return 0;
}

int clone_container_init(struct init_info *info)
{
    if (pipe2(info->pipe_fds, O_CLOEXEC) < 0)
        goto err;
    void *stack = malloc(CHILD_STACK_SIZE);
    stack = (void *)((char *)stack + CHILD_STACK_SIZE);
    if (stack == NULL)
        goto err;
    if ((info->pid = clone(init, stack, CLONE_FLAGS, info)) < 0)
        goto err;
    if (close(info->pipe_fds[0]) < 0)
        goto err;

    return 0;

err:
    print_errno();
    return -1;
}

static
int notify_init(int fd, const char *msg)
{
    int ret = write(fd, msg, strlen(msg));
    if (ret < 0)
        print_errno();
    return ret;
}

int notify_init_proceed(int fd)
{
    return notify_init(fd, PROCEED);
}

int notify_init_fail(int fd)
{
    return notify_init(fd, FAIL);
}

void kill_init(int pid)
{
    if (kill(pid, SIGKILL) < 0)
        print_errno();
}
