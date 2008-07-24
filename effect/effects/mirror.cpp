#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

extern "C" std::string effect_name () { return "Mirror"; }
extern "C" std::string effect_description () { return "Reflects one side upon the other."; }
extern "C" double effect_version () { return 0.5; }
extern "C" bool effect_init (vc::vdFrame & initFrame) { /* Nothing to init. */ return true; }
extern "C" bool effect_deinit () { /* Nothing to deinit. */ return true; }
extern "C" void effect_apply (vc::vdFrame & frame, std::vector<effectArgument> args) {
	for(unsigned int i = 0; i < frame.height; ++i) {
		for(unsigned int j = 0; j < frame.width/2; ++j) {
			vc::pixel temp = frame.getPixel(i,j);
			frame.setPixel(i,frame.width-j-1,temp);
		}
	}
}
