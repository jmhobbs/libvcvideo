#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

extern "C" std::string effect_name () { return "Vertical Mirror"; }
extern "C" std::string effect_description () { return "Reflects the top or bottom half onto the other."; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 0.5; }

extern "C" bool effect_init (vc::vdFrame & initFrame) { return true; }
extern "C" bool effect_deinit () { return true; }

extern "C" void effect_apply (vc::vdFrame & frame, std::vector<effectArgument> args) {
	bool topOnBottom = true;
	for(std::vector<effectArgument>::iterator it = args.begin(); it != args.end(); ++it)
		if(it->name == "Top on Bottom")
			topOnBottom = it->_boolean;

	if(topOnBottom) {
		for(unsigned int i = 0; i < frame.height/2; ++i) {
			for(unsigned int j = 0; j < frame.width; ++j) {
				vc::pixel temp = frame.getPixel(i,j);
				frame.setPixel(frame.height-i-1,j,temp);
			}
		}
	}
	else {
		for(unsigned int i = 0; i < frame.height/2; ++i) {
			for(unsigned int j = 0; j < frame.width; ++j) {
				vc::pixel temp = frame.getPixel(frame.height-i-1,j);
				frame.setPixel(i,j,temp);
			}
		}
	}
}

extern "C" std::vector<effectArgument> effect_arguments () {
	std::vector<effectArgument> ret;
	ret.push_back(
		effectArgument("Top on Bottom","If true, the top is mirrored onto the bottom."
		"If false, the bottom is mirrored onto the top. Default is true.",BOOLEAN)
	);
	return ret;
}
