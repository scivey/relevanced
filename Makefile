.PHONY: run clean thrift thrift-py

clean:
	rm -f src/*.o

thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/RelevancedProtocol.thrift

thrift-py:
	thrift-0.9 --gen py -o ./clients/python/relevanced_client src/RelevancedProtocol.thrift
	rm -rf ./clients/python/relevanced_client/gen_py
	mv ./clients/python/relevanced_client/gen-py ./clients/python/relevanced_client/gen_py

thrift-node:
	thrift-0.9 --gen js:node -o ./clients/nodejs/relevancedClient src/RelevancedProtocol.thrift

thrift-rb:
	thrift-0.9 --gen rb -o ./clients/ruby/relevanced_client src/RelevancedProtocol.thrift


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

.PHONY: doxy serve-docs serve-doxy test-data

test-data:
	mkdir -p testing/tmp
	cd testing/tmp && wget http://qwone.com/~jason/20Newsgroups/20news-18828.tar.gz
	cd testing/tmp && tar -xaf 20news-18828.tar.gz
