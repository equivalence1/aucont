#!/bin/bash

make clean
make
sudo chown 0:0 bin/aucont_start
sudo chmod u+s bin/aucont_start
