CC ?= gcc
CFLAGS ?= -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror
BUILD := build
INCLUDE := -Iinclude
CORE := src/deadlocklab.c

.PHONY: all test clean
all: $(BUILD)/deadlocklab-server

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/deadlocklab-server: $(CORE) src/server.c include/deadlocklab.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) $(CORE) src/server.c -o $@

$(BUILD)/test-deadlocklab: $(CORE) tests/test_deadlocklab.c include/deadlocklab.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) $(CORE) tests/test_deadlocklab.c -o $@

test: $(BUILD)/test-deadlocklab $(BUILD)/deadlocklab-server
	$(BUILD)/test-deadlocklab

clean:
	rm -rf $(BUILD)
