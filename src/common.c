#include <common.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
