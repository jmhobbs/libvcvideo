#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

extern "C" std::string effect_name () { return "Test Pattern"; }
extern "C" std::string effect_description () { return "This is an example libvcvideo plugin. It replaces the frame with a test pattern."; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 1.0; }

extern "C" bool effect_init (vc::vdFrame & initFrame) { /* Nothing to init. */ return true; }
extern "C" bool effect_deinit () { /* Nothing to deinit. */ return true; }
extern "C" void effect_apply (vc::vdFrame & frame, std::vector<effectArgument> args) {
	// Utterly inefficient! Yay!
	for(unsigned int j = 0; j < frame.bufferSize/3; j += 3) {
		frame.buffer[j] = 0;
		frame.buffer[j+1] = 0;
		frame.buffer[j+2] = 255;
	}
	for(unsigned int j = (frame.bufferSize/3); j < (frame.bufferSize/3)*2; j += 3) {
		frame.buffer[j] = 0;
		frame.buffer[j+1] = 255;
		frame.buffer[j+2] = 0;
	}
	for(unsigned int j = (frame.bufferSize/3)*2; j < frame.bufferSize; j += 3) {
		frame.buffer[j] = 255;
		frame.buffer[j+1] = 0;
		frame.buffer[j+2] = 0;
	}
}
