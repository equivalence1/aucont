#ifndef __USER_NS__
#define __USER_NS__

#include "init.h"

int setup_uid_mappings(struct init_info *);
int setup_gid_mappings(struct init_info *);

#endif // __USER_NS__
