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

#ifdef VCVIDEO_DEBUG
	#include <iostream>
	using std::cerr;
	using std::flush;
	using std::endl;
#endif

#include "videoDevice.h"

namespace vc {

	videoDevice::videoDevice (string _deviceName) :
	deviceName(_deviceName), live(false), fd(-1), isV4L2(false), inputCount(0) {}

	videoDevice::~videoDevice () {
		if(fd != -1)
			close(fd);
	}

	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// Setup /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
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
			throw string("Could not open device: "+deviceName);

		if(-1 != ioctl(fd, VIDIOC_QUERYCAP, &v2_capabilities))
			isV4L2 = true;
		else if(-1 == ioctl(fd, VIDIOCGCAP, &v1_capabilities))
			throw string("Could not get device capabilities. Not a V4L or V4L2 device.");

		if(isV4L2) {
			try {
				v2_init();
			}
			catch(string s) {
				throw string(s);
			}
		}
		else {
			try {
				v1_init();
			}
			catch(string s) {
				throw string(s);
			}
		}

		live = true;
	}

	/*!
		Initialize V4L2 devices.

		\throw string On any initialization error.
	*/
	void videoDevice::v2_init () {
		if(!(v2_capabilities.capabilities & V4L2_CAP_VIDEO_CAPTURE))
			throw string("Device '"+deviceName+" not a video capture device.");

		//! \todo Can we use V4L2_CAP_VIDEO_OVERLAY ?

		#ifdef VCVIDEO_DEBUG
		cerr << "----[V4L2 Device Found]---------------------------" << endl;
		cerr << " Name     : " << v2_capabilities.card << endl;
		cerr << " Driver   : " << v2_capabilities.driver << " v." << v2_capabilities.version endl;
		cerr << " Location : " << v2_capabilities.bus_info << endl;
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		// Check out the inputs on the card
		if(-1 == ioctl(fd,VIDIOC_G_INPUT,&currentInput))
			throw string("Can't get the current input on the device '"+deviceName+"'.");

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Enumerate Inputs]----------------------------" << endl;
		#endif
		for(int i = 0; i < MAX_INPUTS; i++) {
			v2_inputs[i].index = i;
			if(-1 == ioctl(fd,VIDIOC_ENUMINPUT,&v2_inputs[i]))
				break;
			++inputCount;
			#ifdef VCVIDEO_DEBUG
			cerr << "  + Input #" << i << endl;
			cerr << "    - Name : " << v2_inputs[i].name << endl;
			cerr << "    - Type : " << ((v2_init[i].type & V4L2_INPUT_TYPE_TUNER) ? "Tuner" : "Camera") << endl;
			#endif
		}
		#ifdef VCVIDEO_DEBUG
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		//! \todo Handle tuners...
		// http://v4l2spec.bytesex.org/spec/x394.htm

		//! \todo Handle input standards specs?
		// http://v4l2spec.bytesex.org/spec/x448.htm

		// Since we a re starting with USB cameras they set v4l2_input std to 0
		// If it isn't, we bail out.
		if(0 != v2_inputs[currentInput].std)
			throw string("Not a USB web camera.");

		// Enumerate input controls
		// http://v4l2spec.bytesex.org/spec/x542.htm
		brightness.id = V4L2_CID_BRIGHTNESS;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,brightness))
			throw string("Could not get control value for brightness.");

		contrast.id = V4L2_CID_CONTRAST;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,contrast))
			throw string("Could not get control value for contrast.");

		saturation.id = V4L2_CID_SATURATION;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,saturation))
			throw string("Could not get control value for saturation.");

		hue.id = V4L2_CID_HUE;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,hue))
			throw string("Could not get control value for hue.");

		//! \todo Finish this.
	}

	/*!
		Initialize V4L1 devices.

		\throw string On any failure to initialize.
	*/
	void videoDevice::v1_init () {

		if(v1_capabilities.type != VID_TYPE_CAPTURE)
			throw string("Device '"+deviceName+" not a video capture device.");

		#ifdef VCVIDEO_DEBUG
		cerr << "----[V4L1 Device Found]---------------------------" << endl;
		cerr << " Name       : " << v1_capabilities.name << endl;
		cerr << " Channels   : " << v1_capabilities.channels << endl;
		cerr << " Max Height : " << v1_capabilities.maxheight << endl;
		cerr << " Max Width  : " << v1_capabilities.maxwidth << endl;
		cerr << " Min Height : " << v1_capabilities.minheight << endl;
		cerr << " Min Width  : " << v1_capabilities.minwidth << endl;
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		//! \todo How about VID_TYPE_OVERLAY?
		//! \todo Need to handle VID_TYPE_SUBCAPTURE at some point.

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Enumerate Channels]--------------------------" << endl;
		#endif
		// Check out the inputs ("channels") on the card
		for(int i = 0; i < v1_capabilities.channels && i < MAX_INPUTS; i++) {
			v1_inputs[i].channel = i;
			if(-1 == ioctl(fd,VIDIOCGCHAN,&v1_inputs[i]))
				break;
			++inputCount;
			#ifdef VCVIDEO_DEBUG
			cerr << "  + Channel #" << i << endl;
			cerr << "    - Number : " << v1_inputs[i].channel << endl;
			cerr << "    - Name   : " << v1_inputs[i].name << endl;
			cerr << "    - Tuners : " << v1_inputs[i].tuners << endl;
			#endif
		}
		#ifdef VCVIDEO_DEBUG
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		// Set to first channel by default?
		if(v1_capabilities.channels > 1)
			if(-1 == ioctl(fd,VIDIOCSCHAN,0))
				throw string("Can't set default channel.");

		if(v1_inputs[0].type != VIDEO_TYPE_CAMERA)
			throw string("This is a V4L1 TV device. We don't handle those yet (they have tuners!).");

		// Get the picture information
		if(-1 == ioctl(fd,VIDIOCGPICT,&v1_controls))
			throw new string("Can't get image properties.");

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Image Properties]----------------------------" << endl;
		cerr << " Brightness: " << v1_controls.brightness << endl;
		cerr << " Hue: " << v1_controls.hue << endl;
		cerr << " Color: " << v1_controls.colour << endl;
		cerr << " Contrast: " << v1_controls.contrast << endl;
		cerr << " Whiteness: " << v1_controls.whiteness << endl;
		cerr << " Depth: "  << v1_controls.depth << endl;
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		// Default with the biggest size capture
		v1_window.width = v1_capabilities.maxwidth;
		v1_window.height = v1_capabilities.maxheight;
		if(-1 == ioctl(fd,VIDIOCSWIN,&v1_window))
			throw string("Tried to set to it's maximum size, but it failed.");
		if(-1 == ioctl(fd,VIDIOCGWIN,&v1_window))
			throw string("Could not get the viewing window.");

		// Check compatible palette and calculate buffer size.
		//! \todo Split this function up into common chunks.

	}

	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// Access ////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	void videoDevice::getFrame (vdFrame * frame) {
		if(!live)
			throw string("Device not initialized.");

		frame->width = v1_window.width;
		frame->height = v1_window.height;
	}

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Controls ///////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	/*!
		Gets whether the control is used on the device.

		\throws string If device not started or an invalid control is specified.

		\param controlType The type of integer control to check.
		\return True if used.
	*/
	bool videoDevice::getIntegerControlUsed(const vdIntegerControl controlType) {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2) {
			switch (controlType) {
				case BRIGHTNESS:
					return !(brightness.flags & V4L2_CTRL_FLAG_DISABLED);
				case CONTRAST:
					return !(contrast.flags & V4L2_CTRL_FLAG_DISABLED);
				case SATURATION:
					return !(saturation.flags & V4L2_CTRL_FLAG_DISABLED);
				case HUE:
					return !(hue.flags & V4L2_CTRL_FLAG_DISABLED);
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
		else {
			switch (controlType) {
				case BRIGHTNESS:
				case CONTRAST:
					return true;
				case SATURATION:
					return false; //! \todo Is saturation == colour?
				case HUE:
					return v1_controls.palette != VIDEO_PALETTE_GREY;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}

		return false;
	}

	/*!
		Get the current setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The brightness value. Put it into context with getBrightnessMin() and getBrightnessMax().
	*/
	int videoDevice::getIntegerControlValue (const vdIntegerControl controlType) {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2) {
			v4l2_control get;

			switch (controlType) {
				case BRIGHTNESS:
					if(brightness.flags & V4L2_CTRL_FLAG_DISABLED)
						throw string("Brightness control not used on this device.");
					get.id = V4L2_CID_BRIGHTNESS;
					break;
				case CONTRAST:
					if(contrast.flags & V4L2_CTRL_FLAG_DISABLED)
						throw string("Contrast control not used on this device.");
					get.id = V4L2_CID_CONTRAST;
					break;
				case SATURATION:
					if(saturation.flags & V4L2_CTRL_FLAG_DISABLED)
						throw new string("Saturation control not used on this device.");
					get.id = V4L2_CID_SATURATION;
					break;
				case HUE:
					if(hue.flags & V4L2_CTRL_FLAG_DISABLED)
						throw new string("Hue control not used on this device.");
					get.id = V4L2_CID_HUE;
					break;
				default:
					throw string("Invalid integer control type.");
					break;
			}

			if(-1 == ioctl(fd,VIDIOC_G_CTRL,get))
				throw string("Could not get the value of the control.");

			return get.value;
		}
		else {
			switch (controlType) {
				case BRIGHTNESS:
					return v1_controls.brightness;
				case CONTRAST:
					return v1_controls.contrast;
				case SATURATION:
					throw string("V4L1 does not support saturation.");
				case HUE:
					if(!getIntegerControlUsed(HUE))
						throw string("Hue control not used on this device.");
					return v1_controls.hue;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the minimum setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The minimum value.
	*/
	int videoDevice::getIntegerControlMinimum (const vdIntegerControl controlType) {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2) {
			v4l2_queryctrl temp;

			switch (controlType) {
				case BRIGHTNESS:
					temp = brightness;
					break;
				case CONTRAST:
					temp = contrast;
					break;
				default:
					throw string("Invalid integer control type.");
					break;
			}

			if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
				throw string("That control not used on this device.");

			return temp.minimum;
		}
		else {
			switch (controlType) {
				case BRIGHTNESS:
				case CONTRAST:
					return 0;
				case SATURATION:
					throw string("V4L1 does not support saturation.");
				case HUE:
					if(!getIntegerControlUsed(HUE))
						throw string("Hue control not used on this device.");
					return 0;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the maximum setting of an integer control, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The maximum value.
	*/
	int videoDevice::getIntegerControlMaximum (const vdIntegerControl controlType) {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2) {
			v4l2_queryctrl temp;

			switch (controlType) {
				case BRIGHTNESS:
					temp = brightness;
					break;
				case CONTRAST:
					temp = contrast;
					break;
				default:
					throw string("Invalid integer control type.");
					break;
			}

			if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
				throw string("That control not used on this device.");

			return temp.maximum;
		}
		else {
			switch (controlType) {
				case BRIGHTNESS:
				case CONTRAST:
					return 65535;
				case SATURATION:
					throw string("V4L1 does not support saturation.");
				case HUE:
					if(!getIntegerControlUsed(HUE))
						throw string("Hue control not used on this device.");
					return 65535;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the minimum effective interval for the brightness setting, if possible.

		\throws string If device not started, the control is not used, or it cannot be retrieved.

		\param controlType The type of integer control to get.
		\return The minimum effective interval for the brightness value.
	*/
	int videoDevice::getIntegerControlStep (const vdIntegerControl controlType) {
		if(!live)
			throw string("Device is not initialized.");

		if(!isV4L2)
			return 1;

		v4l2_queryctrl temp;

		switch (controlType) {
			case BRIGHTNESS:
				temp = brightness;
				break;
			case CONTRAST:
				temp = contrast;
				break;
			default:
				throw string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw string("That control is not used on this device.");

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
			throw string("Device is not initialized.");

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
				throw string("Invalid integer control type.");
				break;
		}

		if(temp.flags & V4L2_CTRL_FLAG_DISABLED)
			throw string("The control is not used on this device.");

		if(-1 == ioctl(fd,VIDIOC_G_CTRL,ctrl))
			throw string("Could not get the value of the control.");

		if(temp.maximum < value || temp.minimum > value)
			throw string("New control value out of range.");

		ctrl.value = value;

		if(-1 == ioctl(fd,VIDIOC_S_CTRL,ctrl))
			throw string("Could not set the value of the control.");

	}

	/*!
		Get the card name

		\throws string If device is not initialized.

		\return The name of the card.
	*/
	string videoDevice::getCardName () {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2)
			return reinterpret_cast<const char *>(v2_capabilities.card);
		else
			return reinterpret_cast<const char *>(v1_capabilities.name);
	}

}
