#define _GNU_SOURCE

#include <pid_ns.h>
#include <common.h>

#include <stdlib.h>
#include <sys/mount.h>
#include <stdio.h>

int mount_proc()
{
    LOG_SETUP;

    if (mount("nodev", "/proc", "proc", 0, NULL) < 0) {
        printf("Could not mount proc\n");
        print_errno();
        return -1;
    }

    return 0;
}
