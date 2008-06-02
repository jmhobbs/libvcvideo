#include <fcntl.h>

#include "videoDevice.h"

namespace vc {

	videoDevice::videoDevice (string _deviceName) :
	deviceName(_deviceName), live(false), fd(-1), inputCount(0) {}

	videoDevice::~videoDevice () {
		if(fd != -1)
			close(fd);
	}


	/*!
		Initialize the device.

		\todo Straighten this up, maybe break it up.
		\todo Get better error strings from ioctl and stuff.

		\throws string Descriptive error string.
	*/
	void videoDevice::init () {
		if(live)
			return;

		fd = open(deviceName.c_str(),O_RDWR);
		if(-1 == fd)
			throw new string("Could not open device: "+deviceName);

		if(-1 == ioctl(fd, VIDIOC_QUERYCAP, &capabilities))
			throw new string("Could not get device capabilities.");

		if!(capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE)
			throw new string("Device '"+deviceName+" not a video capture device.");

		//! \todo Can we use V4L2_CAP_VIDEO_OVERLAY ?
		//! \todo How about audio? V4L2_CAP_AUDIO
		//! \todo Rule out radios? V4L2_CAP_RADIO

		// Check out the inputs on the card
		if(-1 == ioctl(fd,VIDIOC_G_INPUT,&currentInput))
			throw new string("Can't get the current input on the device '"+deviceName+"'.");

		// Enumerate the inputs
		for(int i = 0; i < MAX_INPUTS; i++) {
			inputs[i].index = i;
			if(-1 == ioctl(fd,VIDIOC_ENUMINPUT,&inputs[i]))
				break;
			++inputCount;
		}

		//! \todo Handle tuners...
		// http://v4l2spec.bytesex.org/spec/x394.htm

		//! \todo Handle input standards specs?
		// http://v4l2spec.bytesex.org/spec/x448.htm

		// Since we a re starting with USB cameras they set v4l_input std to 0
		// If it isn't, we bail out.
		if(0 != inputs[currentInput].std)
			throw new string("Not a USB web camera!");

		// Enumerate input controls
		// http://v4l2spec.bytesex.org/spec/x542.htm

		live = true;
	}

	/*!
		Get the card name

		\throws string If device is not initialized.

		\return The name of the card.
	*/
	string videoDevice::getCardName () {
		if(!live)
			throw new string("Device is not initialized.");

		return capabilities.card;
	}

}
