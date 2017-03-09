#ifndef __MOUNT_NS_H__
#define __MOUNT_NS_H__

int mount_rootfs(const char *);
int mount_sysfs();
int get_image_dir(const char *, char *);
int ungz_image(const char *, int, char *);

#endif // __MOUNT_NS_H__
