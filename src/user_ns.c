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

    char uid_map[30];

    if (snprintf(uid_map, sizeof uid_map, "/proc/%d/uid_map", info->pid) < 0)
        goto err;
    if (write_to_file(uid_map, "0 1000 1\n") < 0) {
        pr_err("%s", "Could not setup uid mappings\n");
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}

int setup_gid_mappings(struct init_info *info)
{
    LOG_SETUP;

    char gid_map[30];
    snprintf(gid_map, sizeof gid_map, "/proc/%d/gid_map", info->pid);
    
    if (write_to_file(gid_map, "0 1000 1\n") < 0) {
        pr_err("%s", "Could not setup gid mappings\n");
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}
