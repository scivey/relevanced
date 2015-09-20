CXX=clang++-3.5
CXX_FLAGS=--std=c++14 -stdlib=libstdc++ -I./src
LINK=-lthriftcpp2 -lthrift -lwangle -lfolly -lrocksdb -lglog -lsqlite3 -lz -lsnappy -llz4 -lbz2 -ldouble-conversion -latomic -pthread

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

OBJ=$(addprefix ./src/, \
		main.o \
		englishStopwordSet.o \
		WhitespaceTokenizer.o \
	)

THRIFT_OBJ = $(addprefix ./src/gen-cpp2/, \
		Relevance.o \
		Relevance_client.o \
		Relevance_processmap_binary.o \
		Relevance_processmap_compact.o \
		TextRelevance_constants.o \
		TextRelevance_types.o \
	)

./src/main.o: $(addprefix ./src/, \
		main.cpp data.h Article.h \
		englishStopwordSet.h WhitespaceTokenizer.h \
		Centroid.h CentroidFactory.h RelevanceServer.h \
		RelevanceWorker.h persistence/SqlDb.h persistence/DocumentDBHandle.h \
		persistence/RockHandle.h persistence/CollectionDBHandle.h \
		ProcessedDocument.h \
	)

runner: $(OBJ) $(THRIFT_OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $(OBJ) $(THRIFT_OBJ) $(LINK)

run: runner
	./runner

clean:
	rm -f runner src/*.o

.PHONY: run clean thrift thrift-py

thrift:
	python -m thrift_compiler.main --gen cpp2 -o src src/TextRelevance.thrift

thrift-py:
	thrift1 --gen py -o . src/TextRelevance.thrift
