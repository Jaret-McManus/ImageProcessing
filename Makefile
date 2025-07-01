CXX  := g++
SRCD := src
TSTD := tests
BLDD := build
BIND := bin
INCD := include

EXEC := image
TEST_EXEC := $(EXEC)_tests

MAIN  := $(BLDD)/main.o

ALL_SRCF := $(shell find $(SRCD) -type f -name *.cpp)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.cpp=.o))
ALL_FUNCF := $(filter-out $(MAIN) $(AUX), $(ALL_OBJF))

TEST_SRCF := $(shell find $(TSTD) -type f -name *.cpp)
TEST_OBJF := $(shell find $(TSTD) -type f -name *.o)

INC := -I $(INCD)

CXXFLAGS := -Wall -Werror -Wno-unused-variable -Wno-unused-function -MMD -fcommon
COLORF := -DCOLOR
DFLAGS := -g -DDEBUG -DCOLOR
PGFLAGS := -g -pg
PRINT_STAMENTS := -DERROR -DSUCCESS -DWARN -DINFO

STD := -std=c++20
TEST_LIB := -lcriterion
LIBS := $(LIB)

CXXFLAGS += $(STD)

.PHONY: clean all setup debug

all: setup $(BIND)/$(EXEC) $(BIND)/$(TEST_EXEC)

debug: CXXFLAGS += $(DFLAGS) $(PRINT_STAMENTS) $(COLORF)
debug: all

prof: CXXFLAGS += $(PGFLAGS)
prof: all

setup: $(BIND) $(BLDD)
$(BIND):
	mkdir -p $(BIND)
$(BLDD):
	mkdir -p $(BLDD)

$(BIND)/$(EXEC): $(MAIN) $(ALL_FUNCF)
	$(CXX) $(CXXFLAGS) $(MAIN) $(ALL_FUNCF) -o $@ $(LIBS)

$(BIND)/$(TEST_EXEC): $(ALL_FUNCF) $(TEST_SRCF)
	$(CXX) $(CXXFLAGS) $(INC) $(ALL_FUNCF) $(TEST_SRCF) $(TEST_OBJF) $(TEST_LIB) $(LIBS) -o $@

$(BLDD)/%.o: $(SRCD)/%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

clean:
	rm -rf $(BLDD) $(BIND)
	rm -f gmon.out sandbox-gmon.*

.PRECIOUS: $(BLDD)/*.d
-include $(BLDD)/*.d
