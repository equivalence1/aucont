#!/bin/bash

make clean
make || exit 1
sudo chown 0:0 bin/aucont_start || exit 1
sudo chmod u+s bin/aucont_start || exit 1
