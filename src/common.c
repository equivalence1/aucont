#include <common.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

void print_errno()
{
    printf("%s\n", strerror(errno));
}
