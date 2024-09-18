# Copyright 2017 - 2019, 2023, 2024 Christian Geldermann
#
# This file is part of PeoplezServerLib.
#
# PeoplezServerLib is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# PeoplezServerLib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with PeoplezServerLib.  If not, see <http://www.gnu.org/licenses/>.
#
# Diese Datei ist Teil von PeoplezServerLib.
#
# PeoplezServerLib ist Freie Software: Sie können es unter den Bedingungen
# der GNU General Public License, wie von der Free Software Foundation,
# Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
# veröffentlichten Version, weiterverbreiten und/oder modifizieren.
#
# PeoplezServerLib wird in der Hoffnung, dass es nützlich sein wird, aber
# OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
# Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
# Siehe die GNU General Public License für weitere Details.
#
# Sie sollten eine Kopie der GNU General Public License zusammen mit
# PeoplezServerLib erhalten haben. Wenn nicht, siehe
# <http://www.gnu.org/licenses/>.


# Compilers to use
CC := gcc
CXX := g++
LD := g++
MC := cbmc
VF := /home/christian/git/my_verifast/bin/verifast

# Compilation flags
CXXFLAGS := -std=c++20
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

# Verification parameters
VFARGS := -I /home/christian/git/includes

# Determine source files and '.o'-files
CPPSOURCES := $(shell find $(SOURCEDIR) -name '*.cpp')
CSOURCES := $(shell find $(SOURCEDIR) -name '*.c')
VFSOURCES := String/Parsing/IntToString.cpp System/Alignment.hpp System/IO/Network/Socket.cpp System/IO/Network/SecureSocket.cpp Services/Http/FileType.hpp
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

verify:
	$(foreach f,$(VFSOURCES),echo '' && $(VF) -c -target Linux64 $(VFARGS) src/Peoplez/$(f) &&) echo ''

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
