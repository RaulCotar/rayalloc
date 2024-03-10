# .ONESHELL:
.SHELLFLAGS := -eu -o pipefail -c
MAKEFLAGS += --warn-undefined-variables --no-builtin-rules

VERSION := 0.0.1-dev## Project version.
ADDCFLAGS := ## Additional cmdline-provided compiler flags.
CFLAGS := $(shell sed -E 's/^([^#]*).*$$/\1/' compile_flags.txt | tr '\n' ' ') -UVERSION -DVERSION='"$(VERSION)"' $(ADDCFLAGS)
DEPFLAGS = -MMD -MF $(BUILD_D)/deps/$(*F).d -MT '$(BUILD_D)/deps/$(*F).d $(BUILD_D)/$(*F).o'
DEPFLAGS_T = -MMD -MF $(BUILD_D)/deps/$(*F).test.d -MT '$(BUILD_D)/deps/$(*F).test.d $(BUILD_D)/$(*F).test'
BUILD_D := build## Build directory.
ifndef CC
	CC := cc
endif
ifndef AR
	AR := ar
endif

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c,$(BUILD_D)/%.o,$(SRCS))
TESTS := $(wildcard test/*.c)

.PHONY: test
test: $(patsubst test/%.c,$(BUILD_D)/%.test.out,$(TESTS)) ## Run the test suite.
	@echo -e '\e[2;32mAll tests passed.\e[22;39m Check the out files for more details.'

$(BUILD_D)/librayalloc.a: $(OBJS) ## Create the static library
	$(AR) rcs $@ $^

.PHONY: help
help: ## Generate and print this help message.
	@echo -e "RayAlloc v$(VERSION)"
	@echo -e "This project uses a self-updating Makefile. Dependencies are generated during compilation.\n"
	@{ echo -e '\e[1;4mTarget\e[24m\t\e[4mDescription\e[22;24m';\
		sed -nE 's/^(.*):[[:space:]].*##[[:space:]]+(.*)$$/\1\t\2/p' Makefile ;\
	} | column -dtl2
	@echo ''
	@{ echo -e '\e[1;4mVariable\e[24m\t\e[4mDescription\e[22;24m';\
		sed -nE 's/^(.*):=.*##[[:space:]]+(.*)$$/\1\t\2/p' Makefile ;\
	} | column -dtl2

.PHONY: clean
clean: ## Clean the build directory as well as test results.
	rm -rf $(BUILD_D)

.PRECIOUS: $(BUILD_D)/%.test
$(BUILD_D)/%.test: test/%.c $(BUILD_D)/librayalloc.a
	@echo -e "\e[2;32mCompiling \\e]8;;file://$$(whoami)$$(realpath $(BUILD_D)/$(*F).o)\\e\\\\\e[4m$(BUILD_D)/$(*F).test\e[24m\\e]8;;\\e\\\\\e[22;39m"
# $(CC) $(CFLAGS) $(DEPFLAGS_T) $< $(BUILD_D)/librayalloc.a -o $@
	$(CC) $(CFLAGS) $(DEPFLAGS_T) $< $(OBJS) -o $@

$(BUILD_D)/%.out: $(BUILD_D)/%
	@echo -e "\e[2;32mRunning test \\e]8;;file://$$(whoami)$$(realpath $<)\\e\\\\\e[4m$<\e[24m\\e]8;;\\e\\\\:\e[22;39m "
	@$< > $@ || (echo -e "\e[1;30;41mFAILED\e[22;31;49m with code $$?\e[39m" && false)
	@touch $<

$(BUILD_D)/%.o &: src/%.c compile_flags.txt | buildir
	@echo -e "\e[2;32mCompiling \\e]8;;file://$$(whoami)$$(realpath $(BUILD_D)/$(*F).o)\\e\\\\\e[4m$(BUILD_D)/$(*F).o\e[24m\\e]8;;\\e\\\\\e[22;39m"
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $(BUILD_D)/$(*F).o

-include $(wildcard $(BUILD_D)/deps/*.d)

.PHONY: buildir
buildir: $(BUILD_D)/deps $(BUILD_D)/include

$(BUILD_D)/deps:
	mkdir -p $@

$(BUILD_D)/include: src/include | $(BUILD_D)
	cp -r $< $(BUILD_D)
