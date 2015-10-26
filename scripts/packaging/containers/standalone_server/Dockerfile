FROM phusion/baseimage:0.9.17
RUN apt-get update
RUN apt-get install -y \
        binutils-dev \
        curl \
        libboost-all-dev \
        libboost1.54-all-dev \
        libbz2-dev \
        libdouble-conversion-dev \
        libevent-dev \
        libgflags-dev \
        libgnutls-dev \
        libgoogle-glog-dev \
        libiberty-dev \
        libjemalloc-dev \
        libkrb5-dev \
        liblz4-dev \
        liblzma-dev \
        libnuma-dev \
        libsasl2-dev \
        libsnappy-dev \
        libssl-dev \
        libtool \
        nano \
        pkg-config \
        wget \
        zlib1g-dev

ADD data /data
RUN cp /data/*.deb /data/relevanced.deb && \
        sudo dpkg -i /data/relevanced.deb

EXPOSE 8097
CMD /usr/bin/relevanced --config_file=/etc/relevanced/relevanced.json
