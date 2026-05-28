# =========================================
# Compiler
# =========================================

CXX := g++

# =========================================
# Directories
# =========================================

SRC := src
INCLUDE := include
TEST := tests
BIN := bin

# =========================================
# Output
# =========================================

EXECUTABLE := main
TEST_EXECUTABLE := test

# =========================================
# Extensions
# =========================================

SRC_EXT := cpp
OBJ_EXT := o
DEP_EXT := d

# =========================================
# Compiler Flags
# =========================================

CXX_FLAGS := \
	-Wall \
	-Wextra \
	-std=c++17 \
	-ggdb \
	-MMD \
	-MP

# =========================================
# SDL2
# =========================================

SDL_INCLUDE := -I/opt/homebrew/include

SDL_LIB := -L/opt/homebrew/lib

SDL_LIBRARIES := \
	-lSDL2 \
	-lSDL2_image \
	-lSDL2_ttf

# =========================================
# Include Paths
# =========================================

INCLUDE_FLAGS := \
	-I$(INCLUDE) \
	$(SDL_INCLUDE)

# =========================================
# Libraries
# =========================================

MAIN_LIBRARIES := \
	$(SDL_LIB) \
	$(SDL_LIBRARIES)

TEST_LIBRARIES := \
	$(MAIN_LIBRARIES) \
	-lgtest \
	-lgtest_main \
	-pthread

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

TEST_SOURCES := $(shell find $(TEST) -name '*.cpp' 2>/dev/null)

# =========================================
# Phony Targets
# =========================================

.PHONY: all run clean test rebuild

# =========================================
# Main Build
# =========================================

all: $(BIN)/$(EXECUTABLE)

run: all
	./$(BIN)/$(EXECUTABLE)

rebuild: clean all

# =========================================
# Link Main Executable
# =========================================

$(BIN)/$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BIN)

	$(CXX) \
	$^ \
	$(MAIN_LIBRARIES) \
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
	$(TEST_SOURCES) \
	$(filter-out $(BIN)/main.$(OBJ_EXT), $(OBJECTS)) \
	$(TEST_LIBRARIES) \
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
