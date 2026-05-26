CXX := g++

CXX_FLAGS := \
	-Wall \
	-Wextra \
	-std=c++17 \
	-ggdb \
	-MMD \
	-MP

BIN := bin
SRC := src
INCLUDE := include
TEST := tests

EXECUTABLE := main
TEST_EXECUTABLE := test

SRC_EXT := cpp
OBJ_EXT := o
DEP_EXT := d

# =========================================
# Source Files
# =========================================

SOURCES := $(shell find $(SRC) -name '*.$(SRC_EXT)')

OBJECTS := $(patsubst \
	$(SRC)/%, \
	$(BIN)/%, \
	$(SOURCES:.$(SRC_EXT)=.$(OBJ_EXT)))

DEPS := $(OBJECTS:.$(OBJ_EXT)=.$(DEP_EXT))

# =========================================
# Test Files
# =========================================

TEST_SOURCES := $(shell find $(TEST) -name '*.cpp')

# =========================================
# Include / Library Paths
# =========================================

INCLUDE_FLAGS := \
	-I$(INCLUDE) \
	-I/opt/homebrew/include

LIBRARY_FLAGS := \
	-L/opt/homebrew/lib

# =========================================
# Libraries
# =========================================

LIBRARIES := \
	-lgtest \
	-lgtest_main \
	-pthread

# =========================================
# Phony Targets
# =========================================

.PHONY: all run clean test

# =========================================
# Main Build
# =========================================

all: $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	$(CXX) \
	$(CXX_FLAGS) \
	$(INCLUDE_FLAGS) \
	$^ \
	-o $@

# =========================================
# Object Compilation
# =========================================

$(BIN)/%.$(OBJ_EXT): $(SRC)/%.$(SRC_EXT)
	@mkdir -p $(dir $@)

	$(CXX) \
	$(CXX_FLAGS) \
	$(INCLUDE_FLAGS) \
	-c $< \
	-o $@

# =========================================
# Testing
# =========================================

test: $(OBJECTS)
	@mkdir -p $(BIN)

	$(CXX) \
	$(CXX_FLAGS) \
	$(INCLUDE_FLAGS) \
	$(LIBRARY_FLAGS) \
	$(TEST_SOURCES) \
	$(filter-out $(BIN)/main.o, $(OBJECTS)) \
	$(LIBRARIES) \
	-o $(BIN)/$(TEST_EXECUTABLE)

	./$(BIN)/$(TEST_EXECUTABLE)

# =========================================
# Clean
# =========================================

clean:
	rm -rf $(BIN)

# =========================================
# Dependencies
# =========================================

-include $(DEPS)