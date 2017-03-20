#include <common.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void print_errno()
{
    pr_err("%s\n", strerror(errno));
}

/**
 * Return values:
 * -1 -- some fail occured
 *  0 -- everything is ok
 *  1 -- could not close file
 */
int write_to_file(const char *path, const char *buff)
{
    int fd = open(path, O_RDWR);
    if (fd < 0) {
        pr_err("Could not open file '%s' in read-write mode\n", path);
        goto err;
    }
    if (write(fd, buff, strlen(buff)) < 0) {
        pr_err("Could not write to file '%s'\n", path);
        goto err;
    }
    if (close(fd) < 0) {
        pr_warn("Warning: could not close file '%s'\n", path);
        return 1;
    }

    return 0;

err:
    print_errno();
    return -1;
}

/**
 * @param relative_path -- script's path relative to aucont/bin direcontry
 * @param args -- args of script to execute
 */
int execute_bin_relative(const char *relative_path, const char *args)
{
    char bin_dir[150];
    char run[150];

    memset(bin_dir, 0, sizeof bin_dir);
    memset(run, 0, sizeof run);

    if (readlink("/proc/self/exe", bin_dir, sizeof bin_dir) < 0) {
        pr_err("%s", "Could not resolve path to current executable\n");
        goto err;
    }

    int i = strlen(bin_dir);
    while (bin_dir[i] != '/')
        i--;
    bin_dir[i + 1] = 0;
    snprintf(run, sizeof run, "%s%s %s", bin_dir, relative_path, args);

    if (system(run) != 0) {
        pr_err("Failed to run '%s'\n", run);
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}
