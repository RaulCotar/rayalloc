.ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

CC := clang
CFLAGS := $(shell cat compile_flags.txt|tr '\n' ' ')
TESTS := $(wildcard test/*.c)
ifndef TEST_OUT_REDIR
REDIR_FD = &1
else
REDIR_FD = $$test.out
endif

.PHONY: test
.SILENT: test
test: $(TESTS) librayalloc.a librayalloc.so
	for test in $(TESTS); do
		echo -e "\e[1m\e[36mTEST\e[0m\e[27m $$test\e[0m"
		echo -e '\e[1m-compiling:\e[0m\e[90m '"$$(head -n1 $$test | cut -c1,2 --complement)" $$test -o $$test'.bin\e[0m'
		$$(head -n1 $$test | cut -c1,2 --complement) $$test -o $$test'.bin'
		echo -e "\e[1m-running:\e[0m \e[90m$$test.bin 1>$(REDIR_FD)\e[0m"
		$$test'.bin' 1>$(REDIR_FD) && echo -e '\e[32mPASSED\e[0m' || echo -e '\e[31mFAILED\e[0m'
	done

.SILENT: rayalloc.o
rayalloc.o: rayalloc.c
	echo -e '\e[1m\e[36mBUILDING\e[0m librayalloc'
	echo -e '\e[1m-compiling:\e[0m\e[90m $(CC) $(CFLAGS) -c $< -o $@\e[0m'
	$(CC) $(CFLAGS) -c $< -o $@

.SILENT: librayalloc.a
librayalloc.a: rayalloc.o
	echo -e '\e[1m-static lib:\e[0m\e[90m ar rcs $@ $<\e[0m'
	ar rcs $@ $<

.SILENT: librayalloc.so
librayalloc.so: rayalloc.o
	echo -e '\e[1m-shared lib:\e[0m\e[90m $(CC) $(CFLAGS) -rdynamic -shared $< -o $@\e[0m'
	$(CC) $(CFLAGS) -shared $< -o $@

.PHONY: clean
.SILENT: clean
clean:
	echo -e '\e[1m\e[36mCLEANING\e[0m\e[90m rm -rf *.o *.a $$(ls test/* | grep -xE '.*\.?[^c]')\e[0m'
	rm -rf *.o *.a *.so $$(ls test/* | grep -xE '.*\.?[^c]')

