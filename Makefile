clean:
	rm -f src/*.o

thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/RelevancedProtocol.thrift

thrift-py:
	rm -rf client/python/client/relevanced_client/gen_py
	mkdir -p build/thrift && rm -rf build/thrift/gen-py
	thrift-0.9 --gen py -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-py ./clients/python/client/relevanced_client/gen_py

thrift-node:
	rm -rf ./clients/nodejs/client/gen-nodejs
	mkdir -p build/thrift && rm -rf build/thrift/gen-nodejs
	thrift-0.9 --gen js:node -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-nodejs ./clients/nodejs/client/

thrift-java:
	rm -rf clients/java/client/src/main/java/org/relevanced/client/gen_thrift_protocol
	mkdir -p build/thrift && rm -rf build/thrift/gen-java
	thrift-0.9 --gen java -o build/thrift src/RelevancedProtocol.thrift
	mv ./build/thrift/gen-java/org/relevanced/client/gen_thrift_protocol ./clients/java/client/src/main/java/org/relevanced/client/

thrift-all: thrift thrift-py thrift-node thrift-java

build-docker-standalone:
	cp build/deb/*.deb scripts/packaging/containers/standalone_server/data/
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
	cd build/bin && cmake ../../ && make unit_test_runner -j4 && ./src/test/unit_test_runner

test-rock:
	mkdir -p build/bin
	rm -rf build/bin/test_data && mkdir -p build/bin/test_data
	cd build/bin && cmake ../../ && make rock_handle_test -j4 && ./src/test/rock_handle_test

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

smoketest-node:
	node ./clients/nodejs/client/test/functionalSmoketest.js

smoketest-java:
	cd ./clients/java/client && sbt run

publish-node:
	cd ./clients/nodejs/client/ && npm publish

publish-python:
	cd ./clients/python/client && python setup.py register -r pypi && python setup.py sdist upload -r pypi

format-all:
	find src  \( -name "*.h" -o -name "*.cpp" -o -name "*.mm" \) -exec clang-format -i {} +
