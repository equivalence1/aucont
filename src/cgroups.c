#define _GNU_SOURCE

#include <cgroups.h>
#include <common.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

static const char *HIERARCHY_PATH = "/tmp/aucont_cgroup_cpu";

/*
 * Return values:
 * -1 -- error
 *  0 -- hierarchy was created
 *  1 -- heirarchy was already present
 */
static
int create_cpu_hierarchy()
{
    LOG_SETUP;

    if (mkdir(HIERARCHY_PATH, 0777) < 0) {
        if (errno != EEXIST) {   
            printf("Could not create directory '%s' for cpu cgroup hierarchy\n", HIERARCHY_PATH);
            goto err;
        } else
            return 1;
    }

    return 0;

err:
    print_errno();
    return -1;
}

static
int create_cpu_cg(int init_pid, char *path, int len)
{
    LOG_SETUP;

    char cg_path[strlen(HIERARCHY_PATH) + 15];
    snprintf(cg_path, sizeof cg_path, "%s/cg_%d", HIERARCHY_PATH, init_pid);

    if (mkdir(cg_path, 0777) < 0) {
        printf("Could not create directory '%s' for cpu cgroup\n", cg_path);
        goto err;
    }

    if (len < (int)strlen(cg_path)) {
        printf("string '%s' is longer than %d bytes\n", cg_path, len);
        return -1;
    }

    strcpy(path, cg_path);

    return 0;

err:
    print_errno();
    return -1;
}

static
int mount_cpu_hierarchy()
{
    LOG_SETUP;

    char cmd[200];
    // FIXME cpu and cpuacct might be separated
    snprintf(cmd, sizeof cmd, "sudo mount -t cgroup -o cpu,cpuacct aucont_cpu_cgroup %s", HIERARCHY_PATH);

    if (system(cmd) != 0) {
        printf("Could not mount cgroup hierarchy\n");
        return -1;
    }

    return 0;
}

static
int move_init_to_cpu_cg(int init_pid, char *cg_path)
{
    LOG_SETUP;

    char tasks_path[strlen(cg_path) + 10];
    snprintf(tasks_path, sizeof tasks_path, "%s/tasks", cg_path);
    char pid[10];
    snprintf(pid, sizeof pid, "%d", init_pid);

    if (write_to_file(tasks_path, pid) < 0) {
        printf("Could not add init to a new cgroup\n");
        return -1;
    }

    return 0;
}

static
int set_cpu_percentage(int percentage, char *cg_path)
{
    LOG_SETUP;

    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    int period = 1e+6;
    int quota = percentage * period * cores / 100;

    char period_path[100];
    snprintf(period_path, sizeof period_path, "%s/cpu.cfs_period_us", cg_path);
    char period_str[10];
    snprintf(period_str, sizeof period_str, "%d", period);

    char quota_path[100];
    snprintf(quota_path, sizeof quota_path, "%s/cpu.cfs_quota_us", cg_path);
    char quota_str[10];
    snprintf(quota_str, sizeof quota_str, "%d", quota);

    if (write_to_file(period_path, period_str) < 0)
        return -1;
    if (write_to_file(quota_path, quota_str) < 0)
        return -1;

    return 0;
}

int restrict_cpu_usage(int percentage, int init_pid)
{
    LOG_SETUP;

    int exists = 0;
    char cg_path[100];
    
    if ((exists = create_cpu_hierarchy()) < 0)
        return -1;
    if (exists == 0 && mount_cpu_hierarchy() < 0)
        return -1;
    if (create_cpu_cg(init_pid, cg_path, sizeof cg_path) < 0)
        return -1;
    if (move_init_to_cpu_cg(init_pid, cg_path) < 0)
        return -1;
    if (set_cpu_percentage(percentage, cg_path) < 0)
        return -1;

    return 0;
}
