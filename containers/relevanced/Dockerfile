FROM relevanced/base

RUN cd /build && \
    git clone https://github.com/scivey/relevanced.git && \
    cd relevanced && \
    make thrift && \
    make runner -j4
