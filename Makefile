# Makefile for libvcvideo

# Copyright (C) 2008  John Hobbs - john@velvetcache.org
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

# Compiler setup
CC = g++
CFLAGS = -Wall -g -I. -DVCVIDEO_DEBUG -DSIGCPP `pkg-config --cflags sigc++-2.0` -DHAVE_MAGICK `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`
LFLAGS = -Wall `pkg-config --cflags --libs sigc++-2.0` `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`

# CFLAGS = $(CFLAGS) -DVCVIDEO_DEBUG
# CFLAGS = $(CFLAGS) -DSIGCPP `pkg-config --cflags sigc++-2.0`
# LFLAGS = $(LFLAGS) `pkg-config --cflags --libs sigc++-2.0`
# CFLAGS = $(CFLAGS) -DHAVE_MAGICK `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`
# LFLAGS = $(LFLAGS) `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`

COMPILER = $(CC) $(CFLAGS)

GTKCFLAGS = `pkg-config gtkmm-2.4 --cflags`
GTKLFLAGS = `pkg-config gtkmm-2.4 --cflags --libs`

# Targets
TRGT_TEST_PLAIN = lib/videoDevice.o test/vcvTest.o
testPlain: $(TRGT_TEST_PLAIN)
	$(COMPILER) $(LFLAGS) $(TRGT_TEST_PLAIN) -o $@

TRGT_TEST_GTK = lib/videoDevice.o test/gtkTest.o
testGtk: $(TRGT_TEST_GTK)
	$(COMPILER) $(GTKLFLAGS) $(TRGT_TEST_GTK) -o $@

docs: lib/videoDevice.h lib/videoDevice.cpp docs/tutorial.dox
	cd docs && doxygen config.doxy

clean:
	@rm -f lib/*.o
	@rm -f test/*.o
	@rm -f testPlain
	@rm -f testMagick
	@rm -f testGtk
	@rm -rf docs/html/*

# Object targets
.SUFFIXES : .cpp .o .h

.cpp.o:
	$(COMPILER) -c $< -o $@

depend:
	@echo '# Do not change this file unless you know what you are doing. Use make gendeps instead.' > dependency.mk
	g++ -I. -MM lib/*.cpp | sed 's/^\([a-zA-Z]\)/\nlib\/\1/' >> dependency.mk
	g++ -I. -MM test/*.cpp | sed 's/^\([a-zA-Z]\)/\ntest\/\1/' >> dependency.mk

include dependency.mk

# Special cases...
TRGT_TEST_GTKTEST_O = test/gtkTest.cpp lib/videoDevice.h
test/gtkTest.o: $(TRGT_TEST_GTKTEST_O)
	$(COMPILER) $(GTKCFLAGS) -c test/gtkTest.cpp -o $@