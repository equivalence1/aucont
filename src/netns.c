#define _GNU_SOURCE

#include <netns.h>
#include <common.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int setup_veth_pair(int pid, const char *ip)
{
    LOG_SETUP;

    char bin_dir[120];
    char veth_run[120];
    if (readlink("/proc/self/exe", bin_dir, sizeof bin_dir) < 0) {
        printf("Could not resolve path to current executable\n");
        goto err;
    }

    int i = strlen(bin_dir);
    while (bin_dir[i] != '/')
        i--;
    bin_dir[i + 1] = 0;
    snprintf(veth_run, sizeof veth_run, "%sveth_setup.sh %d %s", bin_dir, pid, ip);

    if (system(veth_run) < 0) {
        printf("Failed to setup veth pair\n");
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}
