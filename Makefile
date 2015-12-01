thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/RelevancedProtocol.thrift

thrift-rb:
	rm -rf ./clients/ruby/client/lib/gen-rb
	thrift-0.9 --gen rb -o build/thrift src/RelevancedProtocol.thrift
	find build/thrift/gen-rb -name *.rb -exec sed -r -i'' -e "s/require 'rel/require_relative 'rel/" {} \;
	mv ./build/thrift/gen-rb ./clients/ruby/client/lib/

thrift-py:
	rm -rf ./clients/python/client/relevanced_client/gen_py
	mkdir -p build/thrift && rm -rf build/thrift/gen-py
	thrift-0.9 --gen py -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-py ./clients/python/client/relevanced_client/gen_py

thrift-node:
	rm -rf ./clients/nodejs/client/gen-nodejs
	mkdir -p build/thrift && rm -rf build/thrift/gen-nodejs
	thrift-0.9 --gen js:node -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-nodejs ./clients/nodejs/client/

thrift-java:
	rm -rf ./clients/java/client/src/main/java/org/relevanced/client/protocol
	mkdir -p build/thrift && rm -rf build/thrift/gen-java
	thrift-0.9 --gen java -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-java/org/relevanced/client/protocol ./clients/java/client/src/main/java/org/relevanced/client/

thrift-all: thrift thrift-py thrift-node thrift-java thrift-rb

build-docker-standalone:
	rm -f scripts/packaging/containers/standalone_server/data/*
	cp build/deb/*.deb scripts/packaging/containers/standalone_server/data/
	sudo docker build -t relevanced/relevanced scripts/packaging/containers/standalone_server

build-docker-2:
	sudo docker build -t relevanced/relevanced scripts/packaging/containers/standalone_server

GTEST_LIB = ./external/gtest-1.7.0-min/gtest-all.o
GMOCK_LIB = ./external/gmock-1.7.0/src/gmock-all.o
WORKDIR=$(shell pwd)

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
	cd build/client_docs && PYTHONPATH=$(WORKDIR)/clients/python/client pdoc --html relevanced_client
	mv build/client_docs/relevanced_client/ build/client_docs/python/

docs:
	mkdocs build --site-dir build/site --clean

test-unit:
	mkdir -p build/bin
	cd build/bin && make unit_test_runner -j4 && ./src/unit_test_runner

test-unit-refresh:
	mkdir -p build/bin
	cd build/bin && cmake ../../ && make unit_test_runner -j4 && ./src/unit_test_runner

test-rock:
	mkdir -p build/bin
	rm -rf build/bin/test_data && mkdir -p build/bin/test_data
	cd build/bin && cmake ../../ && make slow_rock_handle_test -j4 && ./src/slow_rock_handle_test

test-func:
	mkdir -p build/bin
	cd build/bin && make func_test_runner -j4 && ./src/func_test_runner

test-func-refresh:
	mkdir -p build/bin
	cd build/bin && cmake ../../ && make func_test_runner -j4 && ./src/func_test_runner

bench-refresh:
	mkdir -p build/bin
	cd build/bin && cmake ../../ && make bench_runner -j4 && ./src/bench_runner

bench:
	mkdir -p build/bin
	cd build/bin && make bench_runner -j4 && ./src/bench_runner

bench-callgrind: bench
	mkdir -p tmp
	valgrind --tool=callgrind --cacheuse=yes --callgrind-out-file=./tmp/callgrind_report ./build/bin/src/bench_runner
	kcachegrind ./tmp/callgrind_report

.PHONY: test-unit test-rock test-all run package build-server-static

test-all: test-unit test-rock

run:
	mkdir -p build/bin
	rm -rf build/bin/data && mkdir -p build/bin/data
	cd build/bin && cmake ../../ && make relevanced -j4 && ./src/relevanced

build-server-static:
	mkdir -p build/bin
	cd build/bin && cmake ../../ && make relevanced_static -j4

deb-package-local:
	rm -rf build/deb/* && mkdir -p build/deb
	./scripts/packaging/build_deb.sh

deb-package-remote:
	cd scripts/packaging && fab build_relevanced_for_current_branch

docker-sh:
	sudo docker run --rm -t -i relevanced/relevanced /bin/bash

smoketest-java:
	cd ./clients/java/client && sbt run

format-all:
	find src  \( -name "*.h" -o -name "*.cpp" -o -name "*.mm" \) -exec clang-format -i {} +

deps:
	cd external/libstemmer && make libstemmer.o -j4

TEST_DATA_DIR=$(WORKDIR)/build/test_data

run-server:
	mkdir -p $(TEST_DATA_DIR) && rm -rf $(TEST_DATA_DIR)/*
	mkdir -p build/debug && cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../../
	cd build/debug && make relevanced -j4
	cd build/debug && ./src/relevanced --data_dir=$(TEST_DATA_DIR) --logtostderr=1

test-clients:
	$(MAKE) -C clients/python test
	$(MAKE) -C clients/nodejs test
	$(MAKE) -C clients/ruby test
	$(MAKE) -C clients/java test

publish-clients:
	$(MAKE) -C clients/python publish
	$(MAKE) -C clients/nodejs publish
	$(MAKE) -C clients/ruby publish
	$(MAKE) -C clients/java publish

.PHONY: test-clients publish-clients
