# hd5

Get only http server based on solo5 unikernel base.

**Experiment only!**

## Installation

```bash
# Dependencies
sudo apt-get update
sudo apt-get install -y gcc build-essential git qemu-kvm libvirt-bin bridge-utils

# Build 
./build.sh

# Example network config
ip tuntap add tap100 mode tap
ip addr add 10.0.0.1/24 dev tap100
ip link set dev tap100 up

# Execution
./run.sh
```

There is an additional [script](https://github.com/tz70s/hd5/blob/master/tools/iptables.sh) to port-forward into vm instance.

## License
Copyright Tzu-Chiao Yeh 2018, Licensed under MIT.