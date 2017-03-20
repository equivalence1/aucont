#!/usr/bin/env python3

"""
This script should be called with euid = 0.
Some versions of linux (e.g. on my ubuntu) interpreted scripts
can not have suid bit (it's ignored) so in src/ dir there is
a aucont_stop.c which just calls this script and has suid set to 0
(elfs can have suid)
"""

import os
import sys
import signal

pid = 0
sig = 15 # SIGINT

def parse_args(args):
    global pid
    global sig

    if (len(args) > 1) and args[1].isdigit():
        pid = int(args[1])
    else:
        return False

    if (len(args) > 2) and args[2].isdigit():
        sig = int(args[2])

    return True

# Yeah, copy-pasted it from criu-scripts
def kill_tree(pid, sig):
    try:
        f_children_path = "/proc/{0}/task/{0}/children".format(pid)
        f_children = open(f_children_path, "r")
    except:
        sys.stderr.write("Looks like process does not exist already\n")
        return

    try:
        for line in f_children:
            for child_pid in line.strip().split(" "):
                if not child_pid.isdigit():
                    err = ("Error while getting children of {}.\n"
                           "Error occured while parsing '{}'.\n"
                           "Line '{}' could not be parsed correctly.\n"
                           "'{}' is not a valid process id (should be integer).\n")
                    err = err.format(pid, f_children.name, line.strip(), child_pid)
                    raise ValueError(err)
                kill_tree(int(child_pid), sig)

                os.kill(pid, sig)
        os.kill(pid, sig)
    except Exception as e:
        print(e)
    finally:
        f_children.close()


def main(pid, sig):
    try:
        kill_tree(pid, sig)
        os.system("rm -rf /tmp/aucont_image_" + str(pid))
        os.system("rm -r /tmp/aucont_image_" + str(pid) + ".tar.gz")
        os.system("rm -r /var/run/netns/netns_" + str(pid))
        os.system("cgdelete cpu,cpuacct:/cg_" + str(pid))
    except Exception as e:
        sys.stderr.write("Error while cleaning container:\n")
        sys.stderr.write(type(e))
        sys.stderr.write(e)
        return 1


def pr_usage(program_name):
    print("Usage:\n{} <container id> [signal number]\n".format(program_name))


if __name__ == "__main__":
    if not parse_args(sys.argv):
        pr_usage(sys.argv[0])
        sys.exit(1)
    sys.exit(main(pid, sig))
