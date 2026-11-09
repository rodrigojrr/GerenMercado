CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic
BIN := mercado

.PHONY: all run clean

all: $(BIN)

$(BIN): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(BIN)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN) testes
