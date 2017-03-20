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

    int ret;
    char args[30];

    snprintf(args, sizeof args, "%d %s", pid, ip);
    ret = execute_bin_relative("veth_setup.sh", args);

    if (ret < 0)
        pr_err("%s", "Could not setup veth pair\n");

    return ret;
}
