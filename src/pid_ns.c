#define _GNU_SOURCE

#include <pid_ns.h>
#include <stdlib.h>
#include <common.h>

int mount_proc()
{
    LOG_SETUP;
    int ret = system("mount -t proc proc /proc");
    if (ret < 0)
        print_errno();
    return ret;
}
