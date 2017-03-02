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
    if (mkdir(put_old_root, S_IFDIR) < 0) {
        printf("Could not create directory for old root\n");
        goto err;
    }
    if (mount(new_root_path, new_root_path, "bind", MS_BIND | MS_REC, NULL) < 0) {
        printf("Could not bind-mount %s to %s\n", new_root_path, new_root_path);
        goto err;
    }
    if (pivot_root(new_root_path, put_old_root) < 0) {
        printf("Faild to pivot_root\n");
        goto err;
    }
    if (chdir("/") < 0) {
        printf("Could not change directory\n");
        goto err;
    }
    if (chroot("/") < 0) {
        printf("Could not change root directory\n");
        goto err;
    }
    if (umount2(OLD_SUFFIX, MNT_DETACH) < 0) {
        printf("Could not unmount old root\n");
        goto err;
    }
    if (rmdir(OLD_SUFFIX) < 0) {
        printf("Could not delete %s\n", OLD_SUFFIX);
        goto err;
    }

    return 0;

err:
    print_errno();
    return -1;
}

int mount_sysfs()
{
    LOG_SETUP;

    int ret = system("mount -t sysfs sys /sys");
    if (ret < 0)
        print_errno();
    return ret;
}

static
const char *get_image_dir(const char *tmp_dir)
{
    DIR *d = opendir(tmp_dir);
    if (d == NULL) {
        printf("Could not open temp dir (%s)\n", tmp_dir);
        goto err;
    }

    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if (dir->d_name[0] != '.') {
            closedir(d);
            const int len = strlen(tmp_dir) + strlen(dir->d_name) + 2;
            char *image_dir = (char *)malloc(len);
            if (snprintf(image_dir, len, "%s/%s", tmp_dir, dir->d_name) < 0) {
                free(image_dir);
                goto err;
            }
            printf("image dir is '%s'\n", image_dir);
            return image_dir;
        }
    }

    closedir(d);
    printf("image dir is empty\n");

    return NULL;

err:
    print_errno();
    return NULL;
}

const char *ungz_image(const char *img_path)
{
    LOG_SETUP;

    char template[] = "/tmp/aucont_image.XXXXXX";
    char *tmp_dir = mkdtemp(template);
    char untar_command[strlen(img_path) + strlen(tmp_dir) + 15];

    if (tmp_dir == NULL) {
        printf("Could not craete temp dir for image");
        goto err;
    }

    if (snprintf(untar_command, sizeof untar_command, "tar -xzf %s -C %s", img_path, tmp_dir) < 0)
        goto err;
    if (system(untar_command) < 0) {
        printf("Could not extract image\n");
        goto err;
    }

    return get_image_dir(tmp_dir);

err:
    print_errno();
    return NULL;
}
