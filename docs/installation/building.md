# Building

If you have difficulties with the build process, you may be interested in deploying the [containerized version of relevanced](docker.md).

### Dependencies
**relevanced** is built on top of:

- [Boost](http://www.boost.org/)
- [Folly](https://github.com/facebook/folly) and [Wangle](https://github.com/facebook/wangle). (Thread pooling, Futures, utilities)
- Facebook's updated [fbthrift](https://github.com/facebook/fbthrift) branch of [thrift](https://thrift.apache.org/). (The **relevanced** daemon is a Thrift `cpp2` server.)
- [Rocksdb](https://github.com/facebook/rocksdb) (persistence)
- [MITIE](https://github.com/mit-nlp/MITIE) (text processing)

There are scripts for building all of these dependencies.

### Running the build
With the dependencies installed, run:
```bash
mkdir build
cd build && cmake ../
make unit_test_runner # optional
make relevanced
```
