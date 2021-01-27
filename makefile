src = $(wildcard src/*.c)
obj = $(patsubst src/%, obj/%, $(src:.c=.o))
dep = $(obj:.o=.d)
libs = $(patsubst lib%, -l%, $(notdir $(basename $(wildcard libs/*.a) $(wildcard libs/*/*.a)))) \
	-lcunit
exec = $(notdir $(CURDIR))
version = $(cat version)
repo_base = $(shell git rev-parse --show-toplevel)

CFLAGS_INCLUDE_LIB_HEADERS = -Iinc $(patsubst %, -I%, $(dir $(wildcard libs/*/)))
LDFLAGS_INCLUDE_LIB_DIRS = -Llibs $(patsubst %, -L%, $(dir $(wildcard libs/*/)))
CFLAGS = -Wall -ggdb $(CFLAGS_INCLUDE_LIB_HEADERS)
LDFLAGS = $(LDFLAGS_INCLUDE_LIB_DIRS) $(libs)

# for testing, enable conditional compilation macro to compile CUnit tests
ifeq ($(MAKECMDGOALS), test)
	CFLAGS += -D CUNIT_TESTS
endif

all: $(exec)

build: $(exec)

# rule for creating a library in case this is a library project
# an assumption is made that intead of main, a library requires a "lib.c"
# this is used so that this rule isn't accidentally ran on projects that aren't meant to be library projects
lib: $(obj)
ifeq (,$(wildcard src/lib.c))
	$(error "making a static library in a project that doesn't contain a lib.c!")
endif
	mkdir -p build/
	rm -rf build/lib$(exec)-$(version)/
	mkdir -p build/lib$(exec)-$(version)/
	mkdir -p build/lib$(exec)-$(version)/$(exec)/
	$(AR) rcs build/lib$(exec)-$(version)/lib$(exec)-$(version).a $^
	cp inc/* build/lib$(exec)-$(version)/$(exec)/

$(exec): $(obj)
	@mkdir -p build/
	$(CC) $(CFLAGS) -o build/$@-$(version) $^ $(LDFLAGS)

-include $(dep)   # include all dep files in the makefile

obj/%.o: src/%.c
	@mkdir -p obj/
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
obj/%.d: src/%.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.PHONY: clean
clean:
	rm -f $(obj) build/$(exec)
	rm -f $(dep)

.PHONY: run
run: clean $(exec)
	build/$(exec)-$(version) $(args)

.PHONY: test
test: clean $(exec)
	build/$(exec)-$(version) $(args)

# this macro builds a library module and installs it in libs/
# it also takes care of removing old versions of the library from libs/
# (lib_path, lib_version)
define install_lib
	$(MAKE) -C $(1)/ lib
	rm -rf libs/lib$(notdir $(1))-*
	cp -r $(1)/build/lib$(notdir $(1))-$(2) libs/
endef

# rule for installing prerequesites like static libraries before execution
.PHONE: install
install:
	@mkdir -p libs
	$(call install_lib,$(repo_base)/c/cunit_utils,0.1.0)

