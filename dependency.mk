# Do not change this file unless you know what you are doing. Use make gendeps instead.

lib/videoDevice.o: lib/videoDevice.cpp lib/videoDevice.h lib/vdFrame.h

test/gtkTest.o: test/gtkTest.cpp lib/videoDevice.h lib/vdFrame.h

test/vcvTest.o: test/vcvTest.cpp lib/videoDevice.h lib/vdFrame.h

effect/effects.o: effect/effects.cpp effect/effects.h lib/vdFrame.h

effect/effects/example.o: effect/effects/example.cpp lib/vdFrame.h
