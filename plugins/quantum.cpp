#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "lib/effectArgument.h"

#include <stdlib.h>

vc::vdFrame libvcvideo_effect_quantum_frames[8];
short libvcvideo_effect_quantum_counter = 0;

extern "C" std::string effect_name () { return "Quantum"; }
extern "C" std::string effect_description () { return "Does a fuzzy thing with movement. Idea and pseudo code from effectv, 'quark'. http://effectv.sourceforge.net/"; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 1.0; }

extern "C" bool effect_init (vc::vdFrame & initFrame) {
	for(int i = 0; i < 8; i++)
		libvcvideo_effect_quantum_frames[i] = initFrame;
	return true;
}

extern "C" bool effect_deinit () { return true; }

extern "C" void effect_apply (vc::vdFrame & frame, std::vector<vc::effectArgument> args) {

		libvcvideo_effect_quantum_frames[libvcvideo_effect_quantum_counter] = frame;

		for(uint i = 0; i < frame.height; i++)
			for(uint j = 0; j < frame.width; j++)
					frame.setPixel(i,j,libvcvideo_effect_quantum_frames[lrand48()%7].getPixel(i,j));

		if(++libvcvideo_effect_quantum_counter > 7)
			libvcvideo_effect_quantum_counter = 0;
}

extern "C" std::vector<vc::effectArgument> effect_arguments () {
	std::vector<vc::effectArgument> ret;
	return ret;
}
