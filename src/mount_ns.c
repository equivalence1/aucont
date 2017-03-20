#define _GNU_SOURCE

#include <mount_ns.h>
#include <common.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/syscall.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

static
int pivot_root(const char *new_root_path, const char *put_old_root)
{
    return syscall(155, new_root_path, put_old_root);
}

int mount_rootfs(const char *new_root_path)
{
    LOG_SETUP;

    const char *OLD_SUFFIX = "/old_root";
    char put_old_root[strlen(new_root_path) + 10];
    if (snprintf(put_old_root, sizeof put_old_root, "%s%s", new_root_path, OLD_SUFFIX) < 0)
        goto err;
    if (mkdir(put_old_root, 0777) < 0) {
        pr_err("%s", "Could not create directory for old root\n");
        goto err;
    }
    if (mount(new_root_path, new_root_path, "bind", MS_BIND | MS_REC, NULL) < 0) {
        pr_err("Could not bind-mount %s to %s\n", new_root_path, new_root_path);
        goto err;
    }
    if (pivot_root(new_root_path, put_old_root) < 0) {
        pr_err("%s", "Faild to pivot_root\n");
        goto err;
    }
    if (chdir("/") < 0) {
        pr_err("%s", "Could not change directory\n");
        goto err;
    }
    if (chroot("/") < 0) {
        pr_err("%s", "Could not change root directory\n");
        goto err;
    }
    // for some reason we can only mount new proc/sysfs
    // before old root was unmounted
    // TODO: find explanation for this
    if (mount("nodev", "/proc", "proc", 0, NULL) < 0) {
        pr_err("%s", "Could not mount new proc\n");
        goto err;
    }
    if (mount("nodev", "/sys", "sysfs", 0, NULL) < 0) {
        pr_err("%s", "Could not mount new sysfs\n");
        goto err;
    }
    if (umount2(OLD_SUFFIX, MNT_DETACH) < 0) {
        pr_err("%s", "Could not unmount old root\n");
        goto err;
    }
    if (rmdir(OLD_SUFFIX) < 0) {
        pr_err("Could not delete %s\n", OLD_SUFFIX);
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}

int get_image_dir(const char *tmp_dir, char *buff)
{
    DIR *d = opendir(tmp_dir);
    if (d == NULL) {
        pr_err("Could not open temp dir (%s)\n", tmp_dir);
        goto err;
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] != '.') {
            closedir(d);
            const int len = strlen(tmp_dir) + strlen(dir->d_name) + 2;
            if (snprintf(buff, len, "%s/%s", tmp_dir, dir->d_name) < 0)
                goto err;
            pr_info("image dir is '%s'\n", buff);
            return 0;
        }
    }

    closedir(d);
    pr_err("%s", "image dir is empty\n");

    return -1;

err:
    print_errno();
    return -1;
}

int copy_image(const char *img_path, int init_pid, char *buff)
{
    LOG_SETUP;

    char tmp_dir[100]; 
    snprintf(tmp_dir, sizeof tmp_dir, "/tmp/aucont_image_%d", init_pid);
    char tar_command[strlen(img_path) + 200];
    char untar_command[200];

    if (mkdir(tmp_dir, 0777) < 0) {
        pr_err("%s", "Could not craete temp dir for image\n");
        goto err;
    }

    snprintf(tar_command, sizeof tar_command, "tar -czvf /tmp/aucont_image_%d.tar.gz -C %s ../$(basename %s)", init_pid, img_path, img_path);
    if (system(tar_command) != 0) {
        pr_err("%s", "Could not copy image dir\n");
        goto err;
    }

    snprintf(untar_command, sizeof untar_command, "tar -xzvf /tmp/aucont_image_%d.tar.gz -C %s", init_pid, tmp_dir);
    if (system(untar_command) != 0) {
        pr_err("%s", "Could not extract image\n");
        goto err;
    }

    get_image_dir(tmp_dir, buff);

    return 0;

err:
    print_errno();
    return -1;
}
