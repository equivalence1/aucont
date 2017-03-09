#define _GNU_SOURCE

#include <pid_ns.h>
#include <common.h>

#include <stdlib.h>
#include <sys/mount.h>
#include <stdio.h>

int mount_proc()
{
    LOG_SETUP;

    // FIXME for some reason mount("nodev", "/proc", "proc", 0, NULL) gives -EPERM
    // so using system for now
    if (system("/bin/mount -t proc /proc proc") < 0) {
        printf("Could not mount proc\n");
        print_errno();
        return -1;
    }

    return 0;
}
