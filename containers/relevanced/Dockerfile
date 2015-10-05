FROM relevanced/base:0.8a1

RUN cd /build && \
    git clone https://github.com/scivey/relevanced.git && \
    cd relevanced && \
    make thrift && \
    mkdir -p build/bin && \
    cd build/bin && cmake ../../ && \
    LD_LIBRARY_PATH=/usr/local/lib make relevanced -j4
