#ifndef __MOUNT_NS_H__
#define __MOUNT_NS_H__

int mount_rootfs(const char *);
int mount_sysfs();
const char *ungz_image(const char *);

#endif // __MOUNT_NS_H__
