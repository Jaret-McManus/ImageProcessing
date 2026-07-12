CXX       := g++
CXX_FLAGS := -Wall -Wextra -std=c++20 

BIN     := bin
SRC     := src
INCLUDE := include
LIB     := lib
LIBRARIES   := 
EXECUTABLE  := image

SOURCES := $(shell find $(SRC) -name '*.cpp')

all: $(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*
