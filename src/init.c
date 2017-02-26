#define _GNU_SOURCE

#include <init.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static const char *HOSTNAME = "container";
static const size_t CHILD_STACK_SIZE = 8 * 1024 * 1024; // 8MB
static const int CLONE_FLAGS = SIGCHLD
        | CLONE_NEWIPC
        | CLONE_NEWNS
        | CLONE_NEWPID
        | CLONE_NEWUTS
        | CLONE_NEWUSER
        | CLONE_NEWNET;

static int pid;

static
int init(void *args)
{
    sethostname(HOSTNAME, strlen(HOSTNAME));

    char hostname[15];
    gethostname(hostname, 15);

    printf("my hostname is '%s', my pid is %d, my uid is %d\n", hostname, getpid(), getuid());

    return 0;
}

int clone_container_init()
{
    void *stack = malloc(CHILD_STACK_SIZE);
    stack = (void *)((char *)stack + CHILD_STACK_SIZE);
    if (stack == NULL)
        return -1;
    pid = clone(init, stack, CLONE_FLAGS, NULL);
    if (pid < 0)
        return -1;
    return pid;
}
