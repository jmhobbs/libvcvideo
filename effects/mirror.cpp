#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

extern "C" std::string effect_name () { return "Mirror"; }
extern "C" std::string effect_description () { return "Reflects one side upon the other."; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 0.5; }

extern "C" bool effect_init (vc::vdFrame & initFrame) { return true; }
extern "C" bool effect_deinit () { return true; }

extern "C" void effect_apply (vc::vdFrame & frame, std::vector<effectArgument> args) {
	bool leftOnRight = true;
	for(std::vector<effectArgument>::iterator it = args.begin(); it != args.end(); ++it)
		if(it->name == "Left on Right")
			leftOnRight = it->_boolean;

	if(leftOnRight) {
		for(unsigned int i = 0; i < frame.height; ++i) {
			for(unsigned int j = 0; j < frame.width/2; ++j) {
				vc::pixel temp = frame.getPixel(i,j);
				frame.setPixel(i,frame.width-j-1,temp);
			}
		}
	}
	else {
		for(unsigned int i = 0; i < frame.height; ++i) {
			for(unsigned int j = 0; j < frame.width/2; ++j) {
				vc::pixel temp = frame.getPixel(i,frame.width-j-1);
				frame.setPixel(i,j,temp);
			}
		}
	}
}

extern "C" std::vector<effectArgument> effect_arguments () {
	std::vector<effectArgument> ret;
	ret.push_back(
		effectArgument("Left on Right","If true, the left is mirrored "
		"onto the right. If false, the right is mirrored onto the left. Default is true.",BOOLEAN)
	);
	return ret;
}
