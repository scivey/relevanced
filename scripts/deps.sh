#!/bin/bash

sudo apt-get install \
    g++ \
    automake \
    autoconf \
    autoconf-archive \
    libtool \
    libboost-all-dev \
    libevent-dev \
    libdouble-conversion-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    liblz4-dev \
    liblzma-dev \
    libsnappy-dev \
    make \
    zlib1g-dev \
    binutils-dev \
    libjemalloc-dev \
    libssl-dev \
    flex \
    bison \
    libkrb5-dev \
    libsasl2-dev \
    libnuma-dev \
    pkg-config

mkdir -p external/tmp
cd external/tmp && \
    git clone https://github.com/facebook/folly && \
    git clone https://github.com/facebook/fbthrift && \
    git clone https://github.com/facebook/wangle && \
    git clone https://github.com/mit-nlp/MITIE

cd external/tmp/folly && \
    git checkout v0.57.0 && \
    cd folly && autoreconf -ifv && ./configure && \
    make && sudo make install

cd external/tmp/wangle && \
    git checkout v0.13.0 && \
    cd wangle && mkdir build && cd build && \
    cmake ../ && make && sudo make install

cd external/tmp/fbthrift && \
    git checkout v0.31.0 && \
    cd thrift && autoreconf -ifv && cd ./configure && \
    make && sudo make install

cd external/tmp/MITIE && \
    git checkout v0.4 && \
    cd mitielib && \
    make && sudo make install && \
    sudo cp -r include/mitie /usr/local/include/
