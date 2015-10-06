clean:
	rm -f src/*.o

thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/RelevancedProtocol.thrift

thrift-py:
	thrift-0.9 --gen py -o ./clients/python/client/relevanced_client src/RelevancedProtocol.thrift
	rm -rf ./clients/python/client/relevanced_client/gen_py
	mv ./clients/python/client/relevanced_client/gen-py ./clients/python/client/relevanced_client/gen_py

thrift-node:
	thrift-0.9 --gen js:node -o ./clients/nodejs/client/relevancedClient src/RelevancedProtocol.thrift

thrift-java:
	mkdir -p clients/java/src
	mkdir -p build/thrift
	thrift-0.9 --gen java -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-java/org/relevanced/client/gen_thrift_protocol ./clients/java/client/src/org/relevanced/client/

build-docker-base:
	sudo docker build -t relevanced/base containers/base

build-docker-relevanced:
	sudo docker build -t relevanced/relevanced containers/relevanced

GTEST_LIB = ./external/gtest-1.7.0-min/gtest-all.o
GMOCK_LIB = ./external/gmock-1.7.0/src/gmock-all.o

$(GMOCK_LIB): ./external/gmock-1.7.0/src/gmock.cc
	cd ./external/gmock-1.7.0 && autoreconf -ifv && ./configure && make lib/libgmock_main.la

$(GTEST_LIB): ./external/gtest-1.7.0-min/src/gtest_main.cc
	cd ./external/gtest-1.7.0-min && make libgtest.a

doxy:
	mkdir -p build
	doxygen

serve-doxy:
	cd ./build/doxygen && http-server -c-1 -p8013

serve-docs:
	mkdocs serve -a localhost:8014

.PHONY: doxy serve-docs serve-doxy test-data docs-py docs

test-data:
	mkdir -p testing/tmp
	cd testing/tmp && wget http://qwone.com/~jason/20Newsgroups/20news-18828.tar.gz
	cd testing/tmp && tar -xaf 20news-18828.tar.gz

docs-py:
	mkdir -p build/client_docs
	rm -rf build/client_docs/python
	PYTHONPATH=`pwd`/clients/python cd build/client_docs && pdoc --html relevanced_client
	mv build/client_docs/relevanced_client/ build/client_docs/python/

docs:
	mkdocs build --site-dir build/site --clean

test-unit:
	mkdir -p build/bin
	cd build/bin && cmake ../../ && make unit_test_runner -j4 && ./src/test/unit_test_runner

test-rock:
	mkdir -p build/bin
	rm -rf build/bin/test_data && mkdir -p build/bin/test_data
	cd build/bin && cmake ../../ && make rock_handle_test -j4 && ./src/test/rock_handle_test

.PHONY: test-unit test-rock test-all run package

test-all: test-unit test-rock

run:
	mkdir -p build/bin
	rm -rf build/bin/data && mkdir -p build/bin/data
	cd build/bin && cmake ../../ && make relevanced -j4 && ./src/relevanced

package:
	./scripts/packaging/build_deb.sh

