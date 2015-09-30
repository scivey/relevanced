FROM phusion/baseimage:0.9.17
RUN echo base
RUN apt-get install -y software-properties-common && \
    add-apt-repository -y ppa:ubuntu-toolchain-r/test && apt-get update
RUN apt-get install -y git build-essential gcc g++ automake autoconf \
    autoconf-archive libtool libboost-all-dev \
    libevent-dev libdouble-conversion-dev libgoogle-glog-dev libgflags-dev \
    liblz4-dev liblzma-dev libsnappy-dev make zlib1g-dev binutils-dev \
    libjemalloc-dev libssl-dev libiberty-dev

RUN apt-get install -y cmake zip flex libbz2-dev \
    bison scons libkrb5-dev libsasl2-dev libnuma-dev \
    libeigen3-dev liblapack-dev libarpack2-dev \
    wget curl nano
RUN wget -O - http://llvm.org/apt/llvm-snapshot.gpg.key | apt-key add - && \
    apt-get -y install clang-3.5 lldb-3.5

RUN mkdir /build && cd /build && \
    wget https://github.com/facebook/folly/archive/v0.57.0.tar.gz && \
    wget https://github.com/facebook/wangle/archive/v0.13.0.tar.gz && \
    wget https://github.com/facebook/fbthrift/archive/v0.31.0.tar.gz && \
    wget https://github.com/facebook/rocksdb/archive/rocksdb-3.13.1.tar.gz

RUN cd /build && \
    tar -xaf v0.57.0.tar.gz && \
    cd /build/folly-0.57.0/folly && \
    autoreconf -ivf && \
    ./configure && \
    make -j4 && make install

RUN cd /build && \
    tar -xaf v0.13.0.tar.gz && \
    cd /build/wangle-0.13.0/wangle && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make -j4 && make install

RUN cd /build && \
    tar -xaf v0.31.0.tar.gz && \
    cd /build/fbthrift-0.31.0/thrift && \
    autoreconf -ivf && \
    CXX=clang++-3.5 ./configure && \
    make -j8 && make install

RUN cd /build && \
    tar -xaf rocksdb-3.13.1.tar.gz && \
    cd /build/rocksdb-rocksdb-3.13.1 && \
    CXX=clang++-3.5 make shared_lib && \
    make install

RUN cd /build && \
    wget https://github.com/mit-nlp/MITIE/archive/v0.4.tar.gz && \
    tar -xaf v0.4.tar.gz && \
    cd /build/MITIE-0.4 && \
    make -j8

RUN cp -r /build/MITIE-0.4/mitielib/include/* /usr/local/include/ && \
    cp /build/MITIE-0.4/mitielib/libmitie.* /usr/local/lib

