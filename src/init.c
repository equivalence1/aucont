#define _GNU_SOURCE

#include <init.h>
#include <common.h>
#include <uts.h>
#include <pid_ns.h>
#include <mount_ns.h>

#include <user_ns.h>

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
int wait_to_proceed(int fd)
{
    char buff[20];

    if (get_next_msg(fd, buff, sizeof buff) < 0)
        return -1;

    if (!strcmp(PROCEED, buff))
        return 0;
    else {
        printf("While waiting to proceed expected '%s' but found '%s'.\nAboring.\n", PROCEED, buff);
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
        printf("Error while expecting image dir path\n");
        exit(EXIT_FAILURE);
    }
    info->rootfs_path = rootfs_path;

    if (wait_to_proceed(info->pipe_fds[0]) < 0) {
        printf("Can not proceed, aborting\n");
        exit(EXIT_FAILURE);
    }

    if (setup_hostname() < 0) {
        printf("Could not setup new host name\n");
        exit(EXIT_FAILURE);
    }

    if (mount_rootfs(info->rootfs_path) < 0) {
        printf("Could not mount new rootfs\n");
        exit(EXIT_FAILURE);
    }

    if (mount_proc() < 0) {
        printf("Could not mount new proc\n");
        exit(EXIT_FAILURE);
    }

    if (mount_sysfs() < 0) {
        printf("Could not mount new sysfs\n");
        exit(EXIT_FAILURE);
    }



    char hostname[15];
    gethostname(hostname, 15);

    printf("my hostname is '%s', my pid is %d, my uid is %d, my gid is %d, my euid is %d\n", hostname, getpid(), getuid(), getgid(), geteuid());

    execl("/bin/strace", "/bin/strace", NULL);


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
