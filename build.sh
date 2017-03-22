#!/bin/bash

make clean
if [[ $# == 1 && $1 == "debug" ]];
then
    make debug || exit 1
else
    make || exit 1
fi
sudo chown 0:0 bin/aucont_start || exit 1
sudo chmod u+s bin/aucont_start || exit 1
sudo chown 0:0 bin/aucont_stop  || exit 1
sudo chmod u+s bin/aucont_stop  || exit 1
sudo chown 0:0 bin/aucont_exec  || exit 1
sudo chmod u+s bin/aucont_exec  || exit 1
