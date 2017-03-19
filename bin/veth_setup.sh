#!/bin/bash

container_ip=$2
host_ip=$(echo "$2" | awk 'BEGIN {FS="."} {\
                                           for (i = 1; i <= NF; i++) {\
                                              if (i < 4) {\
                                                  printf $i "."\
                                              } else {\
                                                  printf $i + 1\
                                              }\
                                            }\
                                           }')

netns_name="netns_$1"
mkdir -p /var/run/netns
ln -s "/proc/$1/ns/net" "/var/run/netns/$netns_name"

ip link add veth0_$1 type veth peer name veth1                         && echo "veth pair created (veth1 in container and veth0_$1 in host)"     || exit 1
ip link set veth1 netns $netns_name                                    && echo "veth1 moved to netns $netns_name"                                || exit 1
ip netns exec $netns_name ip link set veth1 up                         && echo "veth1 up"                                                        || exit 1
# TODO this ip's can already be used
ip netns exec $netns_name ip address add $container_ip/24 dev veth1    && echo "veth1 ip address set to $container_ip"                           || exit 1
ip link set veth0_$1 up && ip address add $host_ip/24 dev veth0_$1     && echo "veth0 up and ip address set to $host_ip"                         || exit 1
ip netns exec $netns_name ip route add default via $host_ip            && echo "default route throught veth0"                                    || exit 1

ip netns exec $netns_name ip link set lo up                            && echo "loopback device up"                                              || exit 1

echo 1 > /proc/sys/net/ipv4/ip_forward                                 && echo "ip forwarding enabled"                                           || exit 1

if [ "$(iptables -nvL -t nat | grep -P \"MASQUERADE.*all.*wlan0\")" == "" ]; then
    iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE              && echo "nat rule added for wlan0"                                        || exit 1
fi

# TODO for some reason /etc/netns resolv.conf does not work so I'm writing to global resolv.conf for now
#mkdir -p /etc/netns/$netns_name
#touch /etc/netns/$netns_name/resolv.conf
#echo "nameserver 8.8.8.8" > /etc/netns/$netns_name/resolv.conf                    && echo "8.8.8.8 DNS server added for $netns_name"

echo "nameserver 8.8.8.8" > /etc/resolv.conf                           && echo "8.8.8.8 DNS server added in global resolv.conf"                  || exit 1
