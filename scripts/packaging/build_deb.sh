#!/bin/bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

UBUNTU_DEPS="libboost-all-dev (>= 1.54.0),\
            libiberty-dev, \
            libbz2-1.0,  \
            libstdc++6 (>= 4.8),
            libgnutls-dev, \
            libevent-dev,  \
            libdouble-conversion-dev,  \
            libgoogle-glog-dev,  \
            libgflags-dev, \
            liblz4-dev,  \
            liblzma-dev, \
            libsnappy-dev, \
            zlib1g,  \
            libjemalloc-dev, \
            libssl-dev,  \
            libkrb5-dev, \
            libsasl2-dev,  \
            libnuma-dev, \
            pkg-config"

MAKE_SCRIPT="$SCRIPT_DIR/make_linux_package.sh"

$MAKE_SCRIPT -t deb -i 0.9.2 -d $UBUNTU_DEPS
