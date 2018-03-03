#!/bin/bash

fetch() {
  # TODO: Check existed.
  mkdir -p vendor
  git clone https://github.com/Solo5/solo5.git vendor/solo5
}

compile() {
  cd vendor/solo5
  make
}

fetch
compile