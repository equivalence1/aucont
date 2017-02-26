#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>

#define container_of(ptr, type, member) \
    (type *)((char *)(ptr) - offsetof(type, member))

#endif // __COMMON_H__
