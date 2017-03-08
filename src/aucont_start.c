#define _GNU_SOURCE

#include <start_options.h>
#include <init.h>
#include <user_ns.h>
#include <mount_ns.h>
#include <cgroups.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

struct start_opts opts;
struct init_info init;

int main(int argc, char *argv[])
{
    get_start_options(&opts, argc, argv);
    init.opts = &opts;
    const char *rootfs_path;

    rootfs_path = ungz_image(opts.image_path);
    if (rootfs_path == NULL)
        exit(EXIT_FAILURE);
    init.rootfs_path = rootfs_path;

    int ret = clone_container_init(&init);
    if (ret < 0)
        exit(EXIT_FAILURE);

    ret = setup_uid_mappings(&init);
    if (ret < 0)
        goto err;

    // for some reason gid asks for sudo, although uid does not
//    ret = setup_gid_mappings(&init);
    if (ret < 0)
        goto err;

    ret = restrict_cpu_usage(opts.cpu, init.pid);
    if (ret < 0) {
        printf("Could not setup cgroup for cpu usage.\n");
        goto err;
    }

    ret = notify_init_proceed(init.pipe_fds[1]);
    if (ret < 0) {
        printf("Could not notify init to proceed.\nKilling init and aborting.\n");
        goto err;
    }

    int status;
    waitpid(init.pid, &status, 0);

err:
    kill_init(init.pid);
    exit(EXIT_FAILURE);
}
