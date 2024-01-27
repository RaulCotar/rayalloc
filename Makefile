.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

CC := clang
CFLAGS := $(shell tr '\n' ' ' <compile_flags.txt)
SOURCES := $(wildcard src/*.c)
OBJECTS := $(patsubst src/%.c,build/%.o,$(SOURCES))
TEST_TXT := $(patsubst %.c,%.txt,$(wildcard test/[^_]*.c))

.PHONY: quicktest
quicktest: test/_quick.c $(OBJECTS)
	echo -e '\n\n'
	$(CC) $(CFLAGS) $^ -o test/_quick.bin
	./test/_quick.bin

.PHONY: runtest
runtest: $(TEST_TXT)

.PRECIOUS: $(TEST_TXT)
.SILENT: $(TEST_TXT)
test/%.txt: test/%.c build/librayalloc.a
	echo -ne 'Test \e[1m$*\e[22m ...'
	$(CC) $(CFLAGS) $^ -o $($@:.txt=.bin)
	./$($@:.txt=.bin) &>$@ \
		&& (echo "exit code: $$?" &>>$@ && echo -e "\b\b\b\e[32m PASSED\e[39m") \
		|| (echo "exit code: $$?" &>>$@ && echo -e "\b\b\b\e[31m FAILED\e[39m")

build/librayalloc.a: $(OBJECTS) | build
	ar rcs $@ $^

build/%.o: src/%.c build
	$(CC) $(CFLAGS) -c $< -o $@

build: src/include/api
	-mkdir $@ $@/include
	cp -r $</* $@/include/

.PHONY: clean
clean:
	rm -rf build/* test/*.txt test/*.bin
