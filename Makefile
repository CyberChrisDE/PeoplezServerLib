# Compilers to use
CC := gcc
CXX := g++
LD := g++
MC := cbmc
#MC := ~/Downloads/cbmc-5-10-linux-64/cbmc

# Compilation flags
CXXFLAGS := -std=c++1y
CFLAGS := 
CPPFLAGS := -Wall
CPPDEBUG := -O0 -g3 -DDEBUG
CPPRELEASE := -O2 -DNDEBUG

# Linker options
LDFLAGS := 
LDLIBS := -lpthread -lcrypto -lssl -lz -lmysqlcppconn
LDDEBUG := -O0 -g3
LDRELEASE := -O2

# Source/Target folder
SOURCEDIR := src
BUILDDIR := bin

# Determine source files and '.o'-files
CPPSOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
CSOURCES := $(shell find $(SOURCEDIR) -name '*.c')
#SOURCES := $(CSOURCES) $(CPPSOURCES)
OBJS := $(patsubst $(SOURCEDIR)/%.cpp, $(BUILDDIR)/%.o, $(CPPSOURCES)) $(patsubst $(SOURCEDIR)/%.c, $(BUILDDIR)/%.o, $(CSOURCES))

all: clean_objs cpy_dirs
	$(MAKE) release_static
	$(MAKE) clean_objs
	$(MAKE) release_dynamic

release_static: CXXFLAGS += $(CPPRELEASE)
release_static: CFLAGS += $(CPPRELEASE)
release_static: cpy_dirs $(OBJS)
	ar rcs $(BUILDDIR)/libPeoplezServerLib.a $(OBJS)

release_dynamic: CXXFLAGS += $(CPPRELEASE) -fPIC
release_dynamic: CFLAGS += $(CPPRELEASE) -fPIC
release_dynamic: cpy_dirs $(OBJS)
	$(LD) -shared $(LDFLAGS) $(LDRELEASE) $(OBJS) $(LDLIBS) -o $(BUILDDIR)/libPeoplezServerLib.so

debug_static: CXXFLAGS += $(CPPDEBUG)
debug_static: CFLAGS += $(CPPDEBUG)
debug_static: cpy_dirs $(OBJS)
	ar rcs $(BUILDDIR)/libPeoplezServerLib.a $(OBJS)

debug_dynamic: CXXFLAGS += $(CPPDEBUG) -fPIC
debug_dynamic: CFLAGS += $(CPPDEBUG) -fPIC
debug_dynamic: cpy_dirs $(OBJS)
	$(LD) -shared $(LDFLAGS) $(LDDEBUG) $(OBJS) $(LDLIBS) -o $(BUILDDIR)/libPeoplezServerLib.so

cpy_dirs:
	$(shell cd src; find -type d -exec mkdir -p "../bin/{}" \;)

#	SOURCE FILES TO OBJ FILES
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

#	CLEAN UP
clean: clean_objs
	rm -f $(BUILDDIR)/libPeoplezServerLib.a $(BUILDDIR)/libPeoplezServerLib.so
	rm -rf $(BUILDDIR)/*

clean_objs:
	rm -f $(OBJS)

#	Model checking tests
#mc_test:
#	$(MC) mc_tests/Peoplez/String/Parsing/IntToString.cpp src/Peoplez/String/Parsing/IntToString.cpp --function TEST_ToCStringBase10_int32_t --bounds-check --cpp11 -D CBMC
