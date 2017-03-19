#!/bin/bash

if (( $# != 1 ))
then
    echo "should provide PID of container's init"
    exit -1
fi

netns_name="netns_$1"
mkdir -p /var/run/netns
ln -s "/proc/$1/ns/net" "/var/run/netns/$netns_name"

ip link add veth0_$1 type veth peer name veth1                         && echo "veth pair created"
ip link set veth1 netns $netns_name                                    && echo "veth1 moved to netns $netns_name"
ip netns exec $netns_name ip link set veth1 up                         && echo "veth1 up"
# TODO this ip's can already be used
ip netns exec $netns_name ip address add 10.0.0.2/24 dev veth1         && echo "veth1 ip address set"
ip link set veth0_$1 up && ip address add 10.0.0.1/24 dev veth0_$1     && echo "veth0 up and ip address set"
ip netns exec $netns_name ip route add default via 10.0.0.1            && echo "default route throught veth0"

ip netns exec $netns_name ip link set lo up                            && echo "loopback device up"

echo 1 > /proc/sys/net/ipv4/ip_forward                                 && echo "ip forwarding enabled"

iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE                  && echo "nat rule added for wlan0"

# TODO for some reason /etc/netns resolv.conf does not work so I'm writing to global resolv.conf for now
#mkdir -p /etc/netns/$netns_name
#touch /etc/netns/$netns_name/resolv.conf
#echo "nameserver 8.8.8.8" > /etc/netns/$netns_name/resolv.conf                    && echo "8.8.8.8 DNS server added for $netns_name"

echo "nameserver 8.8.8.8" > /etc/resolv.conf
