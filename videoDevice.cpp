/*
	Copyright (C) 2008  John Hobbs - john@velvetcache.org

	This file is part of libvcvideo.

	libvcvideo is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libvcvideo is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libvcvideo.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <fcntl.h>
#include <sys/ioctl.h>

#include <iostream>
using std::cerr;
using std::flush;
using std::endl;

#include "videoDevice.h"

namespace vc {

	videoDevice::videoDevice (string _deviceName) :
	deviceName(_deviceName), live(false), fd(-1), isV4L2(false), inputCount(0) {}

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

		if(-1 != ioctl(fd, VIDIOC_QUERYCAP, &v2_capabilities))
			isV4L2 = true;
		else if(-1 == ioctl(fd, VIDIOCGCAP, &v1_capabilities))
			throw new string("Could not get device capabilities. Not a V4L or V4L2 device.");

		if(isV4L2) {
			try {
				v2_init();
			}
			catch(string * s) {
				throw new string(*s);
			}
		}
		else {
			try {
				v1_init();
			}
			catch(string * s) {
				throw new string(*s);
			}
		}

		live = true;
	}

	void videoDevice::v2_init () {
		if(!(v2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			throw new string("Device '"+deviceName+" not a video capture device.");

		//! \todo Can we use V4L2_CAP_VIDEO_OVERLAY ?

		// Check out the inputs on the card
		if(-1 == ioctl(fd,VIDIOC_G_INPUT,&currentInput))
			throw new string("Can't get the current input on the device '"+deviceName+"'.");

		// Enumerate the inputs
		for(int i = 0; i < MAX_INPUTS; i++) {
			v2_inputs[i].index = i;
			if(-1 == ioctl(fd,VIDIOC_ENUMINPUT,&v2_inputs[i]))
				break;
			++inputCount;
		}

		//! \todo Handle tuners...
		// http://v4l2spec.bytesex.org/spec/x394.htm

		//! \todo Handle input standards specs?
		// http://v4l2spec.bytesex.org/spec/x448.htm

		// Since we a re starting with USB cameras they set v4l2_input std to 0
		// If it isn't, we bail out.
		if(0 != v2_inputs[currentInput].std)
			throw new string("Not a USB web camera.");

		// Enumerate input controls
		// http://v4l2spec.bytesex.org/spec/x542.htm
		brightness.id = V4L2_CID_BRIGHTNESS;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,brightness))
			throw new string("Could not get control value for brightness.");

		contrast.id = V4L2_CID_CONTRAST;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,contrast))
			throw new string("Could not get control value for contrast.");

		saturation.id = V4L2_CID_SATURATION;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,saturation))
			throw new string("Could not get control value for saturation.");

		hue.id = V4L2_CID_HUE;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,hue))
			throw new string("Could not get control value for hue.");
	}

	void videoDevice::v1_init () {
		if(v1_capabilities != VID_TYPE_CAPTURE)
			throw new string("Device '"+deviceName+" not a video capture device.");

		//! \todo How about VID_TYPE_OVERLAY?
		//! \todo Need to handle VID_TYPE_SUBCAPTURE at some point.

		// Check out the inputs ("channels") on the card
		for(int i = 0; i < v1_capabilities.channels && i < MAX_INPUTS; i++) {
			v1_inputs[i].channel = i;
			if(-1 == ioctl(fd,VIDIOCGCHAN,&v1_inputs[i]))
				break;
			++inputCount;
		}

		// Set to first channel by default?
		if(-1 == ioctl(fd,VIDIOCSCHAN,0))
			throw new string("Can't set default channel.");

		// http://www.linuxtv.org/downloads/video4linux/API/V4L1_API.html
	}

	/*!
		Get the current setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The brightness value. Put it into context with getBrightnessMin() and getBrightnessMax().
	*/
	int videoDevice::getIntegerControlValue (const vdIntegerControl controlType) {
		if(!live)
			throw new string("Device is not initialized.");

		v4l2_control get;

		switch (controlType) {
			case BRIGHTNESS:
				if(brightness.flags & V4L2_CTRL_FLAG_DISABLED)
					throw new string("Brightness control not used on this device.");
				get.id = V4L2_CID_BRIGHTNESS;
				break;
			case CONTRAST:
				if(contrast.flags & V4L2_CTRL_FLAG_DISABLED)
					throw new string("Contrast control not used on this device.");
				get.id = V4L2_CID_CONTRAST;
				break;
			default:
				throw new string("Invalid integer control type.");
				break;
		}

		if(-1 == ioctl(fd,VIDIOC_G_CTRL,get))
			throw new string("Could not get the value of the control.");

		return get.value;
	}

	/*!
		Get the minimum setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The minimum value.
	*/
	int videoDevice::getIntegerControlMinimum (const vdIntegerControl controlType) {
		if(!live)
			throw new string("Device is not initialized.");

		v4l2_queryctrl temp;

		switch (controlType) {
			case BRIGHTNESS:
				temp = brightness;
				break;
			case CONTRAST:
				temp = contrast;
				break;
			default:
				throw new string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw new string("That control not used on this device.");

		return temp.minimum;
	}

	/*!
		Get the maximum setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The maximum value.
	*/
	int videoDevice::getIntegerControlMaximum (const vdIntegerControl controlType) {
		if(!live)
			throw new string("Device is not initialized.");

		v4l2_queryctrl temp;

		switch (controlType) {
			case BRIGHTNESS:
				temp = brightness;
				break;
			case CONTRAST:
				temp = contrast;
				break;
			default:
				throw new string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw new string("That control not used on this device.");

		return temp.maximum;
	}

	/*!
		Get the minimum effective interval for the brightness setting, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The minimum effective interval for the brightness value.
	*/
	int videoDevice::getIntegerControlStep (const vdIntegerControl controlType) {
		if(!live)
			throw new string("Device is not initialized.");

		v4l2_queryctrl temp;

		switch (controlType) {
			case BRIGHTNESS:
				temp = brightness;
				break;
			case CONTRAST:
				temp = contrast;
				break;
			default:
				throw new string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw new string("That control is not used on this device.");

		return temp.step;
	}

	/*!
		Sets an integer control on the device, if possible.

		\throws string If device not started, the control is not used, the value is out of range, or it cannot be set.

		\param controlType The integer control to set.
		\param value The new brightness value.
	*/
	void videoDevice::setIntegerControlValue (const vdIntegerControl controlType, const int value) {
		if(!live)
			throw new string("Device is not initialized.");

		v4l2_queryctrl temp;
		v4l2_control ctrl;

		switch (controlType) {
			case BRIGHTNESS:
				temp = brightness;
				ctrl.id = V4L2_CID_BRIGHTNESS;
				break;
			case CONTRAST:
				temp = contrast;
				ctrl.id = V4L2_CID_CONTRAST;
				break;
			default:
				throw new string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw new string("The control is not used on this device.");

		if(-1 == ioctl(fd,VIDIOC_G_CTRL,ctrl))
			throw new string("Could not get the value of the control.");

		if(temp.maximum < value || temp.minimum > value)
			throw new string("New control value out of range.");

		ctrl.value = value;

		if(-1 == ioctl(fd,VIDIOC_S_CTRL,ctrl))
			throw new string("Could not set the value of the control.");

	}

	/*!
		Get the card name

		\throws string If device is not initialized.

		\return The name of the card.
	*/
	string videoDevice::getCardName () {
		if(!live)
			throw new string("Device is not initialized.");

		if(isV4L2)
			return reinterpret_cast<const char *>(v2_capabilities.card);
		else
			return reinterpret_cast<const char *>(v1_capabilities.name);
	}

}
