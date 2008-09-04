# Do not change this file unless you know what you are doing. Use make depend instead.

lib/effects.o: lib/effects.cpp lib/effects.h lib/vdFrame.h \
  lib/effectArgument.h

lib/videoDevice.o: lib/videoDevice.cpp lib/videoDevice.h lib/vdFrame.h

bin/dynTest.o: bin/dynTest.cpp

bin/effectInformation.o: bin/effectInformation.cpp lib/effects.h \
  lib/vdFrame.h lib/effectArgument.h

bin/gtkEffectTest.o: bin/gtkEffectTest.cpp lib/videoDevice.h lib/vdFrame.h \
  lib/effects.h lib/effectArgument.h

bin/gtkTest.o: bin/gtkTest.cpp lib/videoDevice.h lib/vdFrame.h

bin/vcvTest.o: bin/vcvTest.cpp lib/videoDevice.h lib/vdFrame.h

effects/example.o: effects/example.cpp lib/vdFrame.h lib/effectArgument.h

effects/mirror.o: effects/mirror.cpp lib/vdFrame.h lib/effectArgument.h

effects/pixelLapse.o: effects/pixelLapse.cpp lib/vdFrame.h lib/effectArgument.h

effects/quantum.o: effects/quantum.cpp lib/vdFrame.h lib/effectArgument.h

effects/testPattern.o: effects/testPattern.cpp lib/vdFrame.h lib/effectArgument.h

effects/vmirror.o: effects/vmirror.cpp lib/vdFrame.h lib/effectArgument.h
