CXX=clang++-3.5
CXX_FLAGS=--std=c++14 -stdlib=libstdc++ -I./src
LINK=-lfolly -lglog -ldouble-conversion -latomic -pthread

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

OBJ=$(addprefix ./src/, \
		main.o \
		englishStopwordSet.o \
		WhitespaceTokenizer.o \
	)

./src/main.o: $(addprefix ./src/, main.cpp data.h Article.h englishStopwordSet.h WhitespaceTokenizer.h Centroid.h CentroidFactory.h)

runner: $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $(OBJ) $(LINK)

run: runner
	./runner

clean:
	rm -f runner src/*.o

.PHONY: run clean
