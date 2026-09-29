#
# OpenKey engine unit-test makefile (P1).
# Build + run + coverage for the pure C++ engine core.
#
ENGINE_DIR  := ../Sources/OpenKey/engine
HARNESS_DIR := harness
BUILD_DIR   := build

ENGINE_SRCS := $(ENGINE_DIR)/Engine.cpp \
               $(ENGINE_DIR)/Vietnamese.cpp \
               $(ENGINE_DIR)/Macro.cpp \
               $(ENGINE_DIR)/ConvertTool.cpp \
               $(ENGINE_DIR)/SmartSwitchKey.cpp

TEST_SRCS   := $(wildcard engine/*.cpp)

CXX      := clang++
CXXFLAGS := -std=c++17 -O0 -g --coverage -Wall -Wno-deprecated-declarations
INCLUDES := -I$(ENGINE_DIR) -I$(HARNESS_DIR)

BIN := $(BUILD_DIR)/ut_engine

.PHONY: all build test clean coverage

all: build

build:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(TEST_SRCS) $(HARNESS_DIR)/test_config.cpp $(ENGINE_SRCS) -o $(BIN)

test: build
	@$(BIN)

coverage: test
	@cd $(BUILD_DIR) && \
		find . -name '*.gcda' -delete && \
		./ut_engine > /dev/null && \
		echo "--- line coverage per file ---" && \
		for o in Engine Vietnamese Macro ConvertTool SmartSwitchKey; do \
			gcov -o ./ut_engine-$$o.gcno $(ENGINE_DIR)/$$o.cpp 2>/dev/null | grep -A1 "File '$(ENGINE_DIR)/$$o.cpp'" | grep "Lines executed" | sed "s/^/$$o: /"; \
		done

clean:
	rm -rf $(BUILD_DIR)