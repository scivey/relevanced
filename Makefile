CXX=clang++-3.5
CXX_FLAGS=--std=c++14 -stdlib=libstdc++ -I./src
LINK=-lfolly -lglog -latomic -pthread

%.o:%.cpp
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

OBJ=$(addprefix ./src/, \
		main.o \
	)

runner: $(OBJ)
	$(CXX) $(CXX_FLAGS) -o $@ $(OBJ) $(LINK)

run: runner
	./runner

clean:
	rm -f runner src/*.o

.PHONY: run clean
