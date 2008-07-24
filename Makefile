# Makefile for libvcvideo

# Copyright (C) 2008 - SEE "COPYRIGHT" FILE
#
# This file is part of libvcvideo.
#
# libvcvideo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libvcvideo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libvcvideo.  If not, see <http://www.gnu.org/licenses/>.

# Set capabilities...
ifeq ($(SIGCPP),NO)
	SIGCPPCFLAGS =
	SIGCPPLFLAGS =
else
	SIGCPPCFLAGS = -DSIGCPP `pkg-config --cflags sigc++-2.0`
	SIGCPPLFLAGS = `pkg-config --cflags --libs sigc++-2.0`
endif

ifeq ($(VCDEBUG),YES)
	DEBUGCFLAGS = -DVCVIDEO_DEBUG
else
	DEBUGCFLAGS =
endif

ifeq ($(MAGICK),NO)
	MAGICKCFLAGS =
	MAGICKLFLAGS =
else
	MAGICKCFLAGS = -DHAVE_MAGICK `/usr/bin/Magick++-config --cppflags --cxxflags`
	MAGICKLFLAGS = `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`
endif

# Compiler setup
CC = g++
CFLAGS = -Wall -g -fPIC -I. $(DEBUGCFLAGS) $(SIGCPPCFLAGS)
LFLAGS = -Wall $(SIGCPPLFLAGS) $(MAGICKLFLAGS)

COMPILER = $(CC) $(CFLAGS)

GMODULECFLAGS = `pkg-config gmodule-2.0 --cflags`
GMODULEFLAGS = `pkg-config gmodule-2.0 --libs`

# Effect plugins setup
EFFECTFLAGS = -shared -lc

GTKCFLAGS = `pkg-config gtkmm-2.4 --cflags`
GTKLFLAGS = `pkg-config gtkmm-2.4 --cflags --libs`

# Targets
TRGT_TEST_PLAIN = lib/videoDevice.o test/vcvTest.o
testPlain: $(TRGT_TEST_PLAIN)
	$(COMPILER) $(LFLAGS) $(MAGICKLFLAGS) $(TRGT_TEST_PLAIN) -o $@

TRGT_TEST_GTK = lib/videoDevice.o test/gtkTest.o
testGtk: $(TRGT_TEST_GTK)
	$(COMPILER) $(GTKLFLAGS) $(TRGT_TEST_GTK) -o $@

TRGT_TEST_PLUGIN = test/pluginTest.o effect/effects.o
pluginTest: $(TRGT_TEST_PLUGIN)
	$(COMPILER) $(LFLAGS) $(GMODULEFLAGS) $(TRGT_TEST_PLUGIN) -o $@

.PHONY: docs clean

docs:
	cd docs && doxygen config.doxy && echo ".fragment { overflow: auto; }" >> html/doxygen.css #&& cp theme/ftv2folderclosed.png html/

clean:
	@rm -f lib/*.o
	@rm -f test/*.o
	@rm -f testPlain
	@rm -f testGtk
	@rm -rf docs/html/*
	@rm -f effect/*.o
	@rm -f effect/effects/*.o
	@rm -f effect/effects/*.so

# Object targets
.SUFFIXES : .cpp .o .h

.cpp.o:
	$(COMPILER) -c $< -o $@

depend:
	@echo '# Do not change this file unless you know what you are doing. Use make gendeps instead.' > dependency.mk
	g++ -I. -MM lib/*.cpp | sed 's/^\([a-zA-Z]\)/\nlib\/\1/' >> dependency.mk
	g++ -I. -MM test/*.cpp | sed 's/^\([a-zA-Z]\)/\ntest\/\1/' >> dependency.mk
	g++ -I. -MM effect/*.cpp | sed 's/^\([a-zA-Z]\)/\neffect\/\1/' >> dependency.mk
	g++ -I. -MM effect/effects/*.cpp | sed 's/^\([a-zA-Z]\)/\neffect\/effects\/\1/' >> dependency.mk

include dependency.mk

# Special cases...
TRGT_TEST_GTKTEST_O = test/gtkTest.cpp lib/videoDevice.h
test/gtkTest.o: $(TRGT_TEST_GTKTEST_O)
	$(COMPILER) $(GTKCFLAGS) -c $< -o $@

test/vcvTest.o: test/vcvTest.cpp lib/videoDevice.h
	$(COMPILER) $(MAGICKCFLAGS) -c $< -o $@

test/pluginTest.o: test/pluginTest.cpp effect/effects.h
	$(COMPILER) $(GMODULECFLAGS) -c $< -o $@

# Plugins
plugins: effect/effects/example.so effect/effects/testPattern.so effect/effects/mirror.so

effect/effects.o: effect/effects.cpp effect/effects.h
	$(COMPILER) $(GMODULECFLAGS) -c $< -o $@

effect/effects/example.so: effect/effects/example.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effect/effects/testPattern.so: effect/effects/testPattern.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effect/effects/mirror.so: effect/effects/mirror.o
	$(CC) $(EFFECTFLAGS) $< -o $@