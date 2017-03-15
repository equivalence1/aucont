#define _GNU_SOURCE

#include <start_options.h>
#include <init.h>
#include <user_ns.h>
#include <mount_ns.h>
#include <cgroups.h>
#include <netns.h>

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

    setuid(0);
    setgid(0);

    int ret = clone_container_init(&init);
    if (ret < 0)
        exit(EXIT_FAILURE);

    char rootfs_path[100];
    if (ungz_image(opts.image_path, init.pid, rootfs_path) < 0)
        goto err;
    init.rootfs_path = rootfs_path;

    if (send_init(init.pipe_fds[1], rootfs_path) < 0) {
        printf("Could not send image dir path\n");
        goto err;
    }

    ret = setup_uid_mappings(&init);
    if (ret < 0)
        goto err;

    ret = setup_gid_mappings(&init);
    if (ret < 0)
        goto err;

    printf("uid: %d, euid: %d\n", getuid(), geteuid());
    system("id");

    ret = restrict_cpu_usage(opts.cpu, init.pid);
    if (ret < 0) {
        printf("Could not setup cgroup for cpu usage.\n");
        goto err;
    }

    ret = setup_veth_pair(init.pid);
    if (ret < 0)
        goto err;

    ret = notify_init_proceed(init.pipe_fds[1]);
    if (ret < 0) {
        printf("Could not notify init to proceed.\nKilling init and aborting.\n");
        goto err;
    }

    int status;
    waitpid(init.pid, &status, 0);
    exit(EXIT_SUCCESS);

err:
    kill_init(init.pid);
    exit(EXIT_FAILURE);
}
