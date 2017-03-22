#!/bin/bash

# This script should be called with euid = 0.
# Some versions of linux (e.g. on my ubuntu) interpreted scripts
# can not have suid bit (it's ignored) so in src/ dir there is
# a aucont_stop.c which just calls this script and has suid set to 0
# (elfs can have suid)

sig=15
if (( $# == 2 ))
then
    sig=$2
fi

kill -s $sig $1

if [ -e /tmp/aucont_image_$1 ];
then
    rm -rf /tmp/aucont_image_$1
fi

if [ -e /tmp/aucont_image_$1.tar.gz  ];
then
    rm -r /tmp/aucont_image_$1.tar.gz
fi

if [ -e /var/run/netns/netns_$1 ];
then
    rm -r /var/run/netns/netns_$1
fi

sleep 0.1 # for some reason cgdelete need some time after killing process

# cgdelete ALWAYS complains about something, but does its work properly, so > /dev/null
cgdelete cpu,cpuacct:/cg_$1 > /dev/null 2> /dev/null

exit 0
