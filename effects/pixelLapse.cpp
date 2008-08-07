#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "lib/effectArgument.h"

vc::vdFrame libvcvideo_effect_pixelLapse_frame;
unsigned long libvcvideo_effect_pixelLapse_x = 0;
unsigned long libvcvideo_effect_pixelLapse_y = 0;

extern "C" std::string effect_name () { return "Pixel Lapse"; }
extern "C" std::string effect_description () { return "Unusual time lapse style capture. This can be a very slow effect. If you use the random flag it will places the blocks randomly, and be even slower. Idea from http://www.pixel-lapse.com/"; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 1.0; }

extern "C" bool effect_init (vc::vdFrame & initFrame) {
	libvcvideo_effect_pixelLapse_frame = initFrame;
	for(uint i = 0; i < libvcvideo_effect_pixelLapse_frame.bufferSize; ++i)
		libvcvideo_effect_pixelLapse_frame.buffer[i] = 0;
	libvcvideo_effect_pixelLapse_x = 0;
	libvcvideo_effect_pixelLapse_y = 0;
	return true;
}
extern "C" bool effect_deinit () { return true; }

extern "C" void effect_apply (vc::vdFrame & frame, std::vector<vc::effectArgument> args) {
	bool random = false;
	uint blockSize = 32;
	for(std::vector<vc::effectArgument>::iterator it = args.begin(); it != args.end(); ++it) {
		if(it->name == "Random")
			random = it->_boolean;
		if(it->name == "BlockSize")
			blockSize = (it->_integer < 0) ? it->_integer * -1 : it->_integer;
	}
	blockSize = (blockSize == 0) ? 32 : blockSize;

	if(libvcvideo_effect_pixelLapse_frame.bufferSize != frame.bufferSize)
			effect_init(frame);

	for(uint i = 0; i < blockSize && (i+libvcvideo_effect_pixelLapse_y) < frame.height; i++) {
		for(uint j = 0; j < blockSize && (j+libvcvideo_effect_pixelLapse_x) < frame.width; j++) {
			libvcvideo_effect_pixelLapse_frame.setPixel(
				i+libvcvideo_effect_pixelLapse_y,
				j+libvcvideo_effect_pixelLapse_x,
				frame.getPixel(i+libvcvideo_effect_pixelLapse_y,j+libvcvideo_effect_pixelLapse_x)
			);
		}
	}

	if(random) {
		libvcvideo_effect_pixelLapse_x = lrand48()%(frame.width-1);
		libvcvideo_effect_pixelLapse_y = lrand48()%(frame.height-1);
	}
	else {
		libvcvideo_effect_pixelLapse_x += blockSize;
		if(libvcvideo_effect_pixelLapse_x >= frame.width) {
			libvcvideo_effect_pixelLapse_x = 0;
			libvcvideo_effect_pixelLapse_y += blockSize;
			if(libvcvideo_effect_pixelLapse_y>= frame.height)
				libvcvideo_effect_pixelLapse_y = 0;
		}
	}

	frame = libvcvideo_effect_pixelLapse_frame;
}

extern "C" std::vector<vc::effectArgument> effect_arguments () {
	std::vector<vc::effectArgument> ret;
	ret.push_back(
		vc::effectArgument(
			"Random",
			"If true, then it doesn't go linear. It places random blocks on the screen."
			" Can take a long time to get a full frame. Defaults to false.",
			vc::BOOLEAN)
	);
	ret.push_back(
		vc::effectArgument(
			"BlockSize",
			"The size of each replacement block.  The smaller the blocks the slower the"
			" render. Defaults to 32.",
			vc::INTEGER)
	);
	return ret;
}
