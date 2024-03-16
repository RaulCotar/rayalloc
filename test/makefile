# This makefile is made to be included in the project-wide Makefile

TCFLAGS := $(shell sed -E 's/^([^#]*).*$$/\1/' test/compile_flags.txt | tr '\n' ' ')
ADDTCFLAGS :=## Additional cmdline-provided compiler flags (tests).
TESTS := $(wildcard test/*.c)
LOGS := $(patsubst test/%.c,build/test/%.txt,$(TESTS))

.PHONY: test
test: $(LOGS) ## Run all tests.
	@echo -e '\e[2;32m>>> All tests passed.\e[22;39m'

.PRECIOUS: build/test/%
build/test/%: test/%.c libs build/include | build/test
	$(call echo_file_action,Compiling,$@)
	$(CC) $(CFLAGS) $(TCFLAGS) $(ADDTCFLAGS) $< build/librayalloc.a -o $@

build/test/%.txt: build/test/%
	$(call echo_file_action,Executing,$<)
	$< > $@

build/test:
	mkdir -p build/test