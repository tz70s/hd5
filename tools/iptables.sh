#!/bin/bash
#
# Copyright (c) 2018 Tzu-Chiao Yeh
#
# The hd5 project is licensed under MIT License.
# Checkout LICENSE file in the root of source tree.

# Port forward to my vagrant vm.

# Host already forward 8080 -> 80 on guest.
sudo iptables -t nat -A PREROUTING -p tcp -i eth0 --dport 80 -j DNAT --to-destination 10.0.0.2:1234

sudo iptables -A FORWARD -p tcp -d 10.0.0.2 --dport 80 -m state --state NEW,ESTABLISHED,RELATED -j ACCEPT