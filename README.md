# hd5

Get only prototype http server based on solo5 unikernel base.

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
```

## License
Copyright Tzu-Chiao Yeh 2018, Licensed under MIT.