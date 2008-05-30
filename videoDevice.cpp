#include <fctnl.h>

#include "videoDevice.h"

namespace vc {

	videoDevice::videoDevice (string _deviceName) :
	deviceName(_deviceName), live(false), fd(-1), inputCount(0) {}

	videoDevice::~videoDevice () {
		if(fd != -1)
			close(fd);
	}

	//! \todo Check error codes?
	//! \todo Straighten this up, maybe break it up.
	bool videoDevice::open () {
		if(live)
			return true;

		fd = open(deviceName.c_str(),O_RDWR);
		return false;

		if(-1 == ioctl(fd, VIDIOC_QUERYCAP, &capabilities))
			return false;

		if!(capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE)
			return false

		//! \todo Can we use V4L2_CAP_VIDEO_OVERLAY ?
		//! \todo How about audio? V4L2_CAP_AUDIO
		//! \todo Rule out radios? V4L2_CAP_RADIO

		// Check out the inputs on the card
		if(-1 == ioctl(fd,VIDIOC_G_INPUT,&currentInput))
			return false;

		for(int i = 0; i < MAX_INPUTS; i++) {
			inputs[i].index = i;
			if(-1 == ioctl(fd,VIDIOC_ENUMINPUT,&inputs[i]))
				break;
			++inputCount;
		}

		// Left off at tuners...
		// http://v4l2spec.bytesex.org/spec/x394.htm

		live = true;
		return true;
	}

	string videoDevice::getCardName () {
		if(live)
			return capabilities.card;
	}

}