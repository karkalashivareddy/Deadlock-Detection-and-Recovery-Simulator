CC ?= gcc
CFLAGS ?= -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror
BUILD := build
INCLUDE := -Iinclude
CORE := src/deadlocklab.c

.PHONY: all test sanitize clean
all: $(BUILD)/deadlocklab-server

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/deadlocklab-server: $(CORE) src/json.c src/banker.c src/server.c include/deadlocklab.h include/banker.h include/json.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) $(CORE) src/json.c src/banker.c src/server.c -o $@

$(BUILD)/test-deadlocklab: $(CORE) tests/test_deadlocklab.c include/deadlocklab.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) $(CORE) tests/test_deadlocklab.c -o $@

$(BUILD)/test-json: src/json.c tests/test_json.c include/json.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) src/json.c tests/test_json.c -o $@

$(BUILD)/test-banker: $(CORE) src/banker.c tests/test_banker.c include/banker.h include/deadlocklab.h | $(BUILD)
	$(CC) $(CFLAGS) $(INCLUDE) $(CORE) src/banker.c tests/test_banker.c -o $@

test: $(BUILD)/test-deadlocklab $(BUILD)/test-json $(BUILD)/test-banker $(BUILD)/deadlocklab-server
	$(BUILD)/test-deadlocklab
	$(BUILD)/test-json
	$(BUILD)/test-banker

sanitize: | $(BUILD)
	$(CC) -std=c17 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDE) $(CORE) tests/test_deadlocklab.c -o $(BUILD)/test-deadlocklab-sanitize
	$(BUILD)/test-deadlocklab-sanitize
	$(CC) -std=c17 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDE) src/json.c tests/test_json.c -o $(BUILD)/test-json-sanitize
	$(BUILD)/test-json-sanitize
	$(CC) -std=c17 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=address,undefined -fno-omit-frame-pointer $(INCLUDE) $(CORE) src/banker.c tests/test_banker.c -o $(BUILD)/test-banker-sanitize
	$(BUILD)/test-banker-sanitize

clean:
	rm -rf $(BUILD)
