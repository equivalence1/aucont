#include <common.h>

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    setuid(0);
    if (argc < 2) {
        printf("Should provide ID of container\n");
        return -1;
    }
    char args[100];
    if (argc == 2)
        snprintf(args, sizeof args, "%s", argv[1]);
    if (argc == 3)
        snprintf(args, sizeof args, "%s %s", argv[1], argv[2]);
    if (execute_bin_relative("aucont_stop.py", args) < 0) {
        printf("Oops, something went wrong\n");
        return -1;
    }
    return 0;
}
