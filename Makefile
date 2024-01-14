.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

CC := clang
CFLAGS := $(shell tr '\n' ' ' <compile_flags.txt)
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
TEST_SRC := $(wildcard test/[^_]*.c)
TEST_LOG := $(patsubst %.c,%.log,$(TEST_SRC))

.PHONY: quicktest
quicktest: test/_quick.c $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o test/_quick.bin
	./test/_quick.bin

.PHONY: runtest
runtest: $(TEST_LOG)

.PRECIOUS: $(TEST_LOG)
.SILENT: $(TEST_LOG)
test/%.log: test/%.c build/rayalloc.a
	echo -ne 'Test \e[1m$*\e[22m ...'
	sleep 1
	$(CC) $(CFLAGS) $^ -o $(patsubst %.log,%.bin,$@)
	./$(patsubst %.log,%.bin,$@) &>$@ \
		&& (echo "exit code: $$?" &>>$@ && echo -e "\b\b\b\e[32m PASSED\e[39m") \
		|| (echo "exit code: $$?" &>>$@ && echo -e "\b\b\b\e[31m FAILED\e[39m")

build/rayalloc.a: $(OBJECTS)
	ar rcs $@ $^

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -rf build/* test/*.log test/*.bin
