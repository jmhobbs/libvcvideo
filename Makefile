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

MAJOR=1
MINOR=0.2
VERSION=$(MAJOR).$(MINOR)

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
LFLAGS = -Wall $(SIGCPPLFLAGS)

COMPILER = $(CC) $(CFLAGS)

GMODULECFLAGS = `pkg-config gmodule-2.0 --cflags`
GMODULEFLAGS = `pkg-config gmodule-2.0 --libs`

# Effect plugins setup
EFFECTFLAGS = -shared -lc

GTKCFLAGS = `pkg-config gtkmm-2.4 --cflags`
GTKLFLAGS = `pkg-config gtkmm-2.4 --cflags --libs`

# Primary target for making an installation
release: dynamicLib plugins

install: release
	cp lib/libvcvideo.so.$(VERSION) /usr/lib/
	mkdir -p /usr/include/libvcvideo
	cp lib/videoDevice.h /usr/include/libvcvideo/
	cp lib/effects.h /usr/include/libvcvideo/
	cp lib/vdFrame.h /usr/include/libvcvideo/
	mkdir -p /usr/share/libvcvideo/effects/
	cp effects/*.so /usr/share/libvcvideo/effects/
	ldconfig -n /usr/lib/
	rm -f /usr/lib/libvcvideo.so
	ln -s /usr/lib/libvcvideo.so.$(MAJOR) /usr/lib/libvcvideo.so

# Other targets
bins: vcvTest gtkTest effectInformation gtkEffectTest

# Dynamic linked test
bin/dynTest.o: bin/dynTest.cpp
	$(COMPILER) $(GMODULECFLAGS) -I/usr/include/libvcvideo -c $< -o $@

TRGT_BIN_DYN = lib/videoDevice.o bin/dynTest.o
dynTest: $(TRGT_BIN_DYN)
	$(COMPILER) $(LFLAGS) $(GMODULEFLAGS) -lvcvideo $(TRGT_BIN_DYN) -o bin/$@
# End dynamic linked test

TRGT_BIN_PLAIN = lib/videoDevice.o bin/vcvTest.o
vcvTest: $(TRGT_BIN_PLAIN)
	$(COMPILER) $(LFLAGS) $(MAGICKLFLAGS) $(TRGT_BIN_PLAIN) -o bin/$@

TRGT_BIN_GTK = lib/videoDevice.o bin/gtkTest.o
gtkTest: $(TRGT_BIN_GTK)
	$(COMPILER) $(GTKLFLAGS) $(TRGT_BIN_GTK) -o bin/$@

TRGT_BIN_GTK_EFFECT = lib/videoDevice.o bin/gtkEffectTest.o lib/effects.o
gtkEffectTest: $(TRGT_BIN_GTK_EFFECT)
	$(COMPILER) $(GTKLFLAGS) $(TRGT_BIN_GTK_EFFECT) -o bin/$@

TRGT_BIN_EFFECT = bin/effectInformation.o lib/effects.o
effectInformation: $(TRGT_BIN_EFFECT)
	$(COMPILER) $(LFLAGS) $(GMODULEFLAGS) $(TRGT_BIN_EFFECT) -o bin/$@

# Create a static archive
staticLib: lib/videoDevice.o lib/effects.o
	ar -cvr lib/libvcvideo.a lib/videoDevice.o lib/effects.o

# Create a dynamic library
dynamicLib: lib/videoDevice.o lib/effects.o
	$(COMPILER) $(LFLAGS) -shared -Wl,-soname,libvcvideo.so.$(MAJOR) -lc -o lib/libvcvideo.so.$(VERSION) lib/videoDevice.o lib/effects.o

.PHONY: docs clean

# Genetate documentation
docs:
	cd docs && doxygen config.doxy && echo ".fragment { overflow: auto; }" >> html/doxygen.css

# Clean up object files
clean:
	@rm -f lib/*.o
	@rm -f bin/*.o
	@rm -f *.o
	@rm -f effects/*.o

# Clean up _everything_
spotless: clean
	@rm -f bin/vcvTest
	@rm -f bin/gtkTest
	@rm -f bin/dynTest
	@rm -f bin/effectInformation
	@rm -f bin/gtkEffectTest
	@rm -rf docs/html/*
	@rm -f effects/*.so
	@rm -f lib/*.so
	@rm -f lib/*.a
	@rm -f lib/libvcvideo.so.$(VERSION)

# Object targets
.SUFFIXES : .cpp .o .h

.cpp.o:
	$(COMPILER) -c $< -o $@

depend:
	@echo '# Do not change this file unless you know what you are doing. Use make depend instead.' > dependency.mk
	g++ -I. -MM lib/*.cpp | sed 's/^\([a-zA-Z]\)/\nlib\/\1/' >> dependency.mk
	g++ -I. -MM bin/*.cpp | sed 's/^\([a-zA-Z]\)/\nbin\/\1/' >> dependency.mk
	g++ -I. -MM effects/*.cpp | sed 's/^\([a-zA-Z]\)/\neffects\/\1/' >> dependency.mk

include dependency.mk

# Special case dependencies
bin/gtkTest.o: bin/gtkTest.cpp lib/videoDevice.h
	$(COMPILER) $(GTKCFLAGS) -c $< -o $@

bin/gtkEffectTest.o: bin/gtkEffectTest.cpp lib/videoDevice.h lib/effects.h
	$(COMPILER) $(GTKCFLAGS) -c $< -o $@

bin/vcvTest.o: bin/vcvTest.cpp lib/videoDevice.h
	$(COMPILER) $(MAGICKCFLAGS) -c $< -o $@

bin/effectInformation.o: bin/effectInformation.cpp lib/effects.h
	$(COMPILER) $(GMODULECFLAGS) -c $< -o $@

# Included plugins
effects: plugins # Alias
plugins: effects/example.so effects/testPattern.so effects/mirror.so effects/vmirror.so effects/quantum.so

lib/effects.o: lib/effects.cpp lib/effects.h
	$(COMPILER) $(GMODULECFLAGS) -c $< -o $@

effects/example.so: effects/example.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effects/testPattern.so: effects/testPattern.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effects/mirror.so: effects/mirror.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effects/vmirror.so: effects/vmirror.o
	$(CC) $(EFFECTFLAGS) $< -o $@

effects/quantum.so: effects/quantum.o
	$(CC) $(EFFECTFLAGS) $< -o $@
