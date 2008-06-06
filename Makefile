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
# The Mana World Buddy is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libvcvideo.  If not, see <http://www.gnu.org/licenses/>.

# Compiler setup
CC=g++
CFLAGS=-Wall -g -DVCVIDEO_DEBUG
COMPILER=$(CC) $(CFLAGS)

# Targets
lib: videoDevice.o

test: videoDevice.o vcvTest.o
	$(COMPILER) videoDevice.o vcvTest.o -o $@

clean:
	@rm -f *.o

# Object targets
.SUFFIXES : .cpp .o .h

.cpp.o:
	$(COMPILER) -c $< -o $@

gendeps:
	@g++ -D_x86 -D_LINUX -MM *.cpp | sed 's/^\([a-zA-Z]\)/\n\1/' > dependency.mk

include dependency.mk