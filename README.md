# General

Simple implementation of linux containers (like Docker but without its fancy features)

# Compilation

Just run `./build.sh` in the root directory of this repo. Run `./build.sh debug` to compile project with some debug output.

Note that this script will ask you for root password because we need to set suid bit for our binaries after compilation.
All binaries are moved to `bin` directory.

# Tools

## aucont_start

start a new container

usage: `./aucont_start [-d --cpu CPU_PERC --net IP] IMAGE_PATH CMD [ARGS]`

`IMAGE_PATH` - the path to the image of the container's file system (should be just a regular directory)

`CMD` - the command to run inside the container

`ARGS` - arguments for CMD

`-d` - daemonize

`--cpu CPU_PERC` - the percent of the cpu resources allocated for the container 0..100

`--net IP` - create virtual network between host and container.
        IP - container ip address, IP+1 - host side ip address.
        
After a container is started, `aucont_start` will print the ID of the container.

## aucont_stop

Stops the container with the given ID and cleans all its resources.

usage: `./aucont_stop ID [SIG_NUM]`

`SIG_NUM` -- the number of the signal to send to the container's `init`.

## aucont_list

Prints IDs of all of the containers that are currently running.

usage: `./aucont_list`

## aucont_exec

Execute a command inside a container

usage: `./aucont_exec PID CMD [ARGS]`

`PID` - the container's init process pid in its parent PID namespace

`CMD` - the command to run inside the container

`ARGS` - arguments for CMD
