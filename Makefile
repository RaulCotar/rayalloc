.SHELLFLAGS := -eu -o pipefail -c
MAKEFLAGS += --no-builtin-rules --warn-undefined-variables

VERSION := 0.0.5-dev## Project version.
ADDCFLAGS :=## Additional cmdline-provided compiler flags (libs).
CFLAGS := $(shell sed -E 's/^([^#]*).*$$/\1/' compile_flags.txt | tr '\n' ' ') -UVERSION -DVERSION='"$(VERSION)"' $(ADDCFLAGS)
DEPFLAGS = -MMD -MF build/tmp/$(*F).d -MT 'build/tmp/$(*F).d build/tmp/$(*F).o'
CC ?= cc## The C compiler used.
AR ?= ar## The archive program used.
SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,build/tmp/%.o,$(SRCS))

.PHONY: all
all: test ## Default target (build & test for now).

.PHONY: libs
libs: build/librayalloc.a build/librayalloc.so ## Build the static and shared libraries.

build/librayalloc.a: $(OBJS) ## Create the static library.
	$(call echo_file_action,Archiving,$@)
	$(AR) rcs $@ $^

build/librayalloc.so: $(OBJS) ## Create the shared library.
	$(call echo_file_action,Compiling,$@)
	$(CC) -shared $^ -o $@

build/tmp/%.o &: src/%.c compile_flags.txt | build/tmp
	$(call echo_file_action,Compiling,$@)
	$(CC) $(DEPFLAGS) $(CFLAGS) -c $< -o $@

build build/tmp:
	mkdir -p $@

build/include: $(wildcard src/include/*.h) | build ## Create the library headers.
	cp -r src/include build

.PHONY: clean
clean: ## Clean the build directory.
	rm -rf build

-include build/tmp/*.d test/makefile

### Everything below this point is just fluff ###

echo_file_action = @echo -e "\e[2;32m>>> $1 \\e]8;;file://$$(whoami)$$(realpath $2)\\e\\\\\e[4m$2\e[24m\\e]8;;\\e\\\\\e[22;39m"

.PHONY: help
help:
	@echo -e "RayAlloc v$(VERSION)"
	@echo -e "This project uses a self-updating Makefile. Dependencies are generated during compilation.\n"
	@{ echo -e '\e[1;4mTarget\e[24m\t\e[4mDescription\e[22;24m';\
		sed -nE 's/^(.*):[[:space:]].*##[[:space:]]+(.*)$$/\1\t\2/p' Makefile test/makefile ;\
	} | sort | column -dtl2
	@echo ''
	@{ echo -e '\e[1;4mVariable\e[24m\t\e[4mDescription\e[22;24m';\
		sed -nE 's/^(.*)[:?]=.*##[[:space:]]+(.*)$$/\1\t\2/p' Makefile test/makefile ;\
	} | sort | column -dtl2
	@echo -e '\nProbably what you want: \e[2mmake ADDCFLAGS=-Wno-pedantic\e[22m'
