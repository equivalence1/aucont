#define _GNU_SOURCE

#include <pid_ns.h>
#include <common.h>

#include <stdlib.h>

int mount_proc()
{
    LOG_SETUP;
    int ret = system("mount -t proc proc /proc");
    if (ret < 0)
        print_errno();
    return ret;
}
