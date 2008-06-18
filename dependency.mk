# Do not change this file unless you know what you are doing. Use make gendeps instead.

lib/videoDevice.o: lib/videoDevice.cpp lib/videoDevice.h

test/gtkTest.o: test/gtkTest.cpp lib/videoDevice.h

test/vcvTest.o: test/vcvTest.cpp lib/videoDevice.h
