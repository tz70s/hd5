# Copyright (c) 2018 Tzu-Chiao Yeh
#
# The hd5 project is licensed under MIT License.
# Checkout LICENSE file in the root of source tree.

# TODO: Currently, this is based on seperated building.

SOLO5_DIR=vendor/solo5
SOLO5_KERNEL_DIR=$(SOLO5_DIR)/kernel
SOURCE=src
BUILD=build

CC=gcc
MD_CFLAGS=-ffreestanding -mno-red-zone
CFLAGS=$(MD_CFLAGS) -std=c11 -Wall -Wextra -Werror -O2 -g -I$(SOLO5_KERNEL_DIR) -I$(SOURCE) -I$(SOURCE)/sys
LD=ld
LDFLAGS=-nostdlib -z max-page-size=0x1000 -static
OBJCOPY=objcopy

# Network.
NET_SOURCE=$(SOURCE)/net

NET_HEADERS=$(NET_SOURCE)/arp.h \
	$(NET_SOURCE)/config.h \
	$(NET_SOURCE)/ether.h \
	$(NET_SOURCE)/ip.h \
	$(NET_SOURCE)/icmp.h \
	$(NET_SOURCE)/utils.h \
	$(NET_SOURCE)/udp.h \
	$(NET_SOURCE)/tcp.h \
	$(NET_SOURCE)/http.h \
	$(NET_SOURCE)/service.h

HEADERS=$(SOLO5_KERNEL_DIR)/solo5.h $(SOURCE)/sys/libc.h $(NET_HEADERS)

OBJFILES=$(BUILD)/service.o \
	$(BUILD)/utils.o \
	$(BUILD)/icmp.o \
	$(BUILD)/arp.o \
	$(BUILD)/udp.o \
	$(BUILD)/tcp.o \
	$(BUILD)/app.o

all: pre_build hd5.virtio

hd5.virtio: $(OBJFILES)
	$(LD) -T $(SOLO5_KERNEL_DIR)/virtio/solo5.lds \
	$(LDFLAGS) -o $@ $(SOLO5_KERNEL_DIR)/virtio/solo5.o $(SOLO5_KERNEL_DIR)/lib.o $^ $(LDLIBS)

$(BUILD)/%.o: $(SOURCE)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(NET_SOURCE)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

pre_build:
	@mkdir -p build

# Build solo5 core.
# Can be appended on the pre-build.
pre_build_solo5:
	$(MAKE) -C $(SOLO5_DIR)

.PHONY: clean

clean:
	rm -rf build
	rm -f hd5.virtio
