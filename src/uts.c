#define _GNU_SOURCE

#include <uts.h>
#include <common.h>
#include <string.h>
#include <unistd.h>

static const char *HOSTNAME = "container";

int setup_hostname()
{
    LOG_SETUP;

    int ret = sethostname(HOSTNAME, strlen(HOSTNAME));
    if (ret < 0)
        print_errno();
    return ret;
}
