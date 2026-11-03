CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude
SRC := $(wildcard src/*.cpp)
BIN := mercado

.PHONY: all run test clean

all: $(BIN)

$(BIN): $(SRC) $(wildcard include/*.h)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

run: $(BIN)
	./$(BIN)

test: $(BIN)
	$(CXX) $(CXXFLAGS) tests/testes.cpp $(filter-out src/main.cpp,$(SRC)) -o testes
	./testes

clean:
	rm -f $(BIN) testes
