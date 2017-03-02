#define _GNU_SOURCE

#include <user_ns.h>
#include <common.h>

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

int setup_uid_mappings(struct init_info *info)
{
    LOG_SETUP;

    int uid;
    char buff[30];
    
    if ((uid = getuid()) < 0) {
        printf("Could not get your current user id.\n");
        goto err;
    }

    if (snprintf(buff, sizeof buff, "/proc/%d/uid_map", info->pid) < 0)
        goto err;
    int fd = open(buff, O_WRONLY);
    if (fd < 0)
        goto err;
    if (snprintf(buff, sizeof buff, "%d %d 1", 0, uid) < 0)
        goto err;
    if (write(fd, buff, strlen(buff)) < 0)
        goto err;
    if (close(fd) < 0)
        goto err;

    return 0;

err:
    print_errno();
    return -1;
}

int setup_gid_mappings(struct init_info *info)
{
    LOG_SETUP;

    int gid;
    char buff[30];
    
    if ((gid = getgid()) < 0) {
        printf("Could not get your current user group id.\n");
        goto err;
    }

    if (snprintf(buff, sizeof buff, "/proc/%d/gid_map", info->pid) < 0)
        goto err;
    int fd = open(buff, O_WRONLY);
    if (fd < 0)
        goto err;
    if (snprintf(buff, sizeof buff, "%d %d 1", 0, gid) < 0)
        goto err;
    if (write(fd, buff, strlen(buff)) < 0)
        goto err;
    if (close(fd) < 0)
        goto err;

    return 0;

err:
    print_errno();
    return -1;
}
