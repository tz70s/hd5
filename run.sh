#!/bin/bash
#
# Copyright (c) 2018 Tzu-Chiao Yeh
#
# The hd5 project is licensed under MIT License.
# Checkout LICENSE file in the root of source tree.

args=("$@")

remain_args() {
  shift
}

virtio_instance() {
  remain_args
  # FIXME: The argument can't be correctly passed.
  source vendor/solo5/tools/run/solo5-run-virtio.sh -n tap100 hd5.virtio ${args}
}

virtio_instance
