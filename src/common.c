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
    printf("%s\n", strerror(errno));
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
        printf("Could not open file '%s' in read-write mode\n", path);
        goto err;
    }
    if (write(fd, buff, strlen(buff)) < 0) {
        printf("Could not write to file '%s'\n", path);
        goto err;
    }
    if (close(fd) < 0) {
        printf("Warning: could not close file '%s'\n", path);
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
    char bin_dir[200];
    char run[200];
    if (readlink("/proc/self/exe", bin_dir, sizeof bin_dir) < 0) {
        printf("Could not resolve path to current executable\n");
        goto err;
    }

    printf("bin_dir: %s\n", bin_dir);
    int i = strlen(bin_dir);
    while (bin_dir[i] != '/')
        i--;
    bin_dir[i + 1] = 0;
    printf("bin_dir: %s\n", bin_dir);
    snprintf(run, sizeof run, "%s%s %s", bin_dir, relative_path, args);

    if (system(run) != 0) {
        printf("Failed to run '%s'\n", run);
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}
