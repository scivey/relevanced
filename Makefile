CXX=clang++-3.5
CC=clang-3.5
CFLAGS=-I./src --std=c99
CXX_FLAGS=--std=c++14 -stdlib=libstdc++ -I./src -I./external/gmock-1.7.0/include -I./external/gtest-1.7.0/include -O0
LINK=-lthriftcpp2 -lthrift -lwangle -lfolly -lrocksdb -lmitie -lglog -lz -lsnappy -llz4 -lbz2 -ldouble-conversion -lboost_thread -lboost_system -ljemalloc -latomic -pthread

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

LIB_OBJ=$(addprefix ./src/, \
		DocumentProcessingWorker.o \
		DocumentProcessor.o \
		ThriftRelevanceServer.o \
		RelevanceServer.o \
		CentroidUpdateWorker.o \
		CentroidUpdater.o \
		Centroid.o \
		SimilarityScoreWorker.o \
		tokenizer/Tokenizer.o \
		stemmer/PorterStemmer.o \
		stopwords/StopwordFilter.o \
		stopwords/english_stopwords.o \
		persistence/Persistence.o \
		persistence/SyncPersistence.o \
		persistence/PrefixedRockHandle.o \
		persistence/InMemoryRockHandle.o \
		persistence/RockHandle.o \
		util.o \
	)

THRIFT_OBJ = $(addprefix ./src/gen-cpp2/, \
		Relevance.o \
		Relevance_client.o \
		Relevance_processmap_binary.o \
		Relevance_processmap_compact.o \
		TextRelevance_constants.o \
		TextRelevance_types.o \
	)

MAIN_OBJ=./src/main.o $(LIB_OBJ)

relevanced: $(MAIN_OBJ) $(THRIFT_OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $(MAIN_OBJ) $(THRIFT_OBJ) $(LINK)

run: relevanced
	./relevanced

clean:
	rm -f runner src/*.o src/persistence/*.o src/stemmer/*.o src/stopwords/*.o src/tokenizer/*.o

.PHONY: run clean thrift thrift-py

thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/TextRelevance.thrift

thrift-py:
	thrift-0.9 --gen py -o ./clients/python/relevanced_client src/TextRelevance.thrift
	rm -rf ./clients/python/relevanced_client/gen_py
	mv ./clients/python/relevanced_client/gen-py ./clients/python/relevanced_client/gen_py

thrift-node:
	thrift-0.9 --gen js:node -o ./clients/nodejs/relevancedClient src/TextRelevance.thrift

thrift-rb:
	thrift-0.9 --gen rb -o ./clients/ruby/relevanced_client src/TextRelevance.thrift


.PHONY: proc

build-docker-base:
	sudo docker build -t scivey/cpp-base containers/cpp-base

build-docker-relevanced:
	sudo docker build -t scivey/relevanced containers/relevanced

GTEST_LIB = ./external/gtest-1.7.0-min/gtest-all.o
GMOCK_LIB = ./external/gmock-1.7.0/src/gmock-all.o

$(GMOCK_LIB): ./external/gmock-1.7.0/src/gmock.cc
	cd ./external/gmock-1.7.0 && autoreconf -ifv && ./configure && make lib/libgmock_main.la

$(GTEST_LIB): ./external/gtest-1.7.0-min/src/gtest_main.cc
	cd ./external/gtest-1.7.0-min && make libgtest.a

UNIT_TEST_OBJ = $(addprefix ./src/test_unit/, \
		test_PorterStemmer.o \
		test_DocumentSerialization.o \
		test_StopwordFilter.o \
		test_Tokenizer.o \
		test_DocumentProcessor.o \
		test_CentroidDBHandle.o \
		test_DocumentDBHandle.o \
		test_ClassifierDBHandle.o \
		test_InMemoryRockHandle.o \
		runTests.o \
	)

unit_test_runner: $(UNIT_TEST_OBJ) $(LIB_OBJ) $(THRIFT_OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $(UNIT_TEST_OBJ) $(THRIFT_OBJ) $(LIB_OBJ) $(GTEST_LIB) $(GMOCK_LIB) $(LINK)

test-unit: unit_test_runner
	./unit_test_runner

clean-test:
	rm -f src/test_unit/*.o
