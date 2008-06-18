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
CFLAGS = -Wall -g -I.
LFLAGS = -Wall

ifeq ($(VCVIDEO_DEBUG),YES)
  CFLAGS = $(CFLAGS) -DVCVIDEO_DEBUG
endif

ifeq ($(SIGCPP),YES)
  CFLAGS = $(CFLAGS) -DSIGCPP `pkg-config --cflags sigc++-2.0`
  LFLAGS = $(LFLAGS) `pkg-config --cflags --libs sigc++-2.0`
endif

ifeq ($(HAVEMAGICK),YES)
  CFLAGS = $(CFLAGS) -DHAVE_MAGICK `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`
  LFLAGS = $(LFLAGS) `/usr/bin/Magick++-config --cppflags --cxxflags --ldflags --libs`
endif

COMPILER = $(CC) $(CFLAGS)

GTKCFLAGS = `pkg-config gtkmm-2.4 --cflags`
GTKLFLAGS = `pkg-config gtkmm-2.4 --cflags --libs`

# Targets
TRGT_TEST_PLAIN = lib/videoDevice.o test/vcvTest.o
testPlain: $(TRGT_TEST_PLAIN)
	$(COMPILER) $(TRGT_TEST_PLAIN) -o $@

TRGT_TEST_MAGICK = lib/videoDevice.o test/vcvTest.o
testMagick:
	$(COMPILER) $(TRGT_TEST_MAGICK) -o $@

TRGT_TEST_GTK = lib/videoDevice.o test/gtkTest.o
testGtk:
	$(COMPILER) $(GTKLFLAGS) $(TRGT_TEST_GTK) -o $@

clean:
	@rm -f lib/*.o
	@rm -f test/*.o
	@rm -f testPlain
	@rm -f testMagick
	@rm -f testGtk

# Object targets
.SUFFIXES : .cpp .o .h

.cpp.o:
	$(COMPILER) -c $< -o $@

gendeps:
	@echo '# Do not change this file unless you know what you are doing. Use make gendeps instead.' > dependency.mk
	g++ -I. -MM lib/*.cpp | sed 's/^\([a-zA-Z]\)/\nlib\/\1/' >> dependency.mk
	g++ -I. -MM test/*.cpp | sed 's/^\([a-zA-Z]\)/\ntest\/\1/' >> dependency.mk

include dependency.mk

# Special cases...
TRGT_TEST_GTKTEST_O = test/gtkTest.cpp lib/videoDevice.h
test/gtkTest.o: $(TRGT_TEST_GTKTEST_O)
	$(COMPILER) $(GTKCFLAGS) -c $(TRGT_TEST_GTKTEST_O) -o $@