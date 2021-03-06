/*
	Copyright (C) 2008 - SEE "COPYRIGHT" FILE

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

// Device enumeration includes
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


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
		\throws string Pass through from v2_init() and v1_init()
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
		cerr << " Driver   : " << v2_capabilities.driver << " v." << v2_capabilities.version << endl;
		cerr << " Location : " << v2_capabilities.bus_info << endl;
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		// Check out the inputs on the card
		if(-1 == ioctl(fd,VIDIOC_G_INPUT,&currentInput))
			throw string("Can't get the current input on the device '"+deviceName+"'.");

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Enumerate Inputs]----------------------------" << endl;
		#endif
		for(int i = 0; i < VC_V4L2_MAX_INPUTS; i++) {
			v2_inputs[i].index = i;
			if(-1 == ioctl(fd,VIDIOC_ENUMINPUT,&v2_inputs[i]))
				break;
			++inputCount;
			#ifdef VCVIDEO_DEBUG
			cerr << "  + Input #" << i << endl;
			cerr << "    - Name : " << v2_inputs[i].name << endl;
			cerr << "    - Type : " << ((v2_inputs[i].type & V4L2_INPUT_TYPE_TUNER) ? "Tuner" : "Camera") << endl;
			#endif
		}
		#ifdef VCVIDEO_DEBUG
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		//! \todo Handle tuners...
		// http://v4l2spec.bytesex.org/spec/x394.htm

		//! \todo Handle input standards specs?
		// http://v4l2spec.bytesex.org/spec/x448.htm

		// Since we are starting with USB cameras they set v4l2_input std to 0
		// If it isn't, we bail out.
		if(0 != v2_inputs[currentInput].std)
			throw string("Device '"+deviceName+"' not a USB web camera.");

		// Enumerate input controls
		// http://v4l2spec.bytesex.org/spec/x542.htm
		brightness.id = V4L2_CID_BRIGHTNESS;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,brightness)) {
			brightness.flags = V4L2_CTRL_FLAG_DISABLED;
			#ifdef VCVIDEO_DEBUG
			cerr << "Could not get control value for brightness." << end;
			#endif
			//throw string("Could not get control value for brightness.");
		}

		contrast.id = V4L2_CID_CONTRAST;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,contrast)) {
			contrast.flags = V4L2_CTRL_FLAG_DISABLED;
			#ifdef VCVIDEO_DEBUG
			cerr << "Could not get control value for contrast." << end;
			#endif
			//throw string("Could not get control value for contrast.");
		}

		saturation.id = V4L2_CID_SATURATION;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,saturation)) {
			saturation.flags = V4L2_CTRL_FLAG_DISABLED;
			#ifdef VCVIDEO_DEBUG
			cerr << "Could not get control value for saturation." << end;
			#endif
			//throw string("Could not get control value for saturation.");
		}

		hue.id = V4L2_CID_HUE;
		if(-1 == ioctl(fd,VIDIOC_QUERYCTRL,hue)) {
			hue.flags = V4L2_CTRL_FLAG_DISABLED;
			#ifdef VCVIDEO_DEBUG
			cerr << "Could not get control value for hue." << end;
			#endif
			//throw string("Could not get control value for hue.");
		}

		// Extended controls...?

		// Format negotiation
		//! \todo FIXME
// 		format.type = currentInput.type;
// 		if(-1 == ioctl(fd,VIDIOC_G_FMT,format))
// 			throw string("Could not access available output formats.");

		//! \todo Enumerate possible formats here, right now just take what we are given
		#ifdef VCVIDEO_DEBUG
		cerr << "Format - FourCC: " << char * (format.pixelformat) << endl;
		{
			v4l2_fmtdesc fmtDesc;
			fmtDesc.index = 1;
			fmtDesc.pixelformat = format.pixelformat;
			if(-1 != ioctl(fd,VIDIOC_ENUM_FMT,fmtDesc))
				cerr << "Format -   Name: " << fmtDesc.description << endl;
		}
		#endif

		//! \todo Cropping/Scaling

		//! \todo Streaming parameters
		// High quality mode? VIDIOC_S_PARM - V4L2_MODE_HIGHQUALITY

		//! \todo Finish this.
		throw string("V4L2 Not yet supported.");
	}

	/*!
		Initialize V4L1 devices.

		\throw string On any failure to initialize.
	*/
	void videoDevice::v1_init () {

		#ifdef SIGCPP
		sig_progress.emit(10,"Checking Type");
		#endif

		if(v1_capabilities.type != VID_TYPE_CAPTURE)
			throw string("Device '"+deviceName+" not a video capture device.");

		#ifdef SIGCPP
		sig_progress.emit(20,"Enumerating Channels");
		#endif

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
		for(int i = 0; i < v1_capabilities.channels && i < VC_V4L_MAX_INPUTS; i++) {
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

		#ifdef SIGCPP
		sig_progress.emit(30,"Setting Default Channel");
		#endif

		// Set to first channel by default?
		if(v1_capabilities.channels > 1)
			if(-1 == ioctl(fd,VIDIOCSCHAN,0))
				throw string("Can't set default channel.");

		#ifdef SIGCPP
		sig_progress.emit(40,"Checking Camera Class");
		#endif

		if(v1_inputs[0].type != VIDEO_TYPE_CAMERA)
			throw string("This is a V4L1 TV device. We don't handle those yet (They have tuners!).");

		#ifdef SIGCPP
		sig_progress.emit(50,"Getting Properties");
		#endif

		// Get the picture information
		if(-1 == ioctl(fd,VIDIOCGPICT,&v1_controls))
			throw string("Can't get image properties.");

		#ifdef SIGCPP
		sig_progress.emit(60,"Getting Region");
		#endif

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Image Properties]----------------------------" << endl;
		cerr << " Brightness : " << v1_controls.brightness << endl;
		cerr << " Hue        : " << v1_controls.hue << endl;
		cerr << " Color      : " << v1_controls.colour << endl;
		cerr << " Contrast   : " << v1_controls.contrast << endl;
		cerr << " Whiteness  : " << v1_controls.whiteness << endl;
		cerr << " Depth      : " << v1_controls.depth << endl;
		cerr << " Palette    : " << v1_paletteName(v1_controls.palette) << endl;
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		if(-1 == ioctl(fd,VIDIOCGWIN,&v1_window))
			throw string("Could not get the viewing window.");

		#ifdef SIGCPP
		sig_progress.emit(70,"Checking Dimensions");
		#endif

		#ifdef VCVIDEO_DEBUG
		cerr << "----[Video Sizes]---------------------------------" << endl;
		#endif
		// This is from gspca stuff, better method available?
		#define POS_DIMS 14
		int sizes[POS_DIMS] = {640,480,384,288,352,288,320,240,192,144,176,144,160,120};

		for(int j = 0; j < POS_DIMS-1; j++) {

			if(sizes[j] > v1_capabilities.maxwidth)
				continue;
			if(sizes[j] < v1_capabilities.minwidth)
				break;

			#ifdef SIGCPP
			//! \todo This doesn't work very well. Not accurate.
			sig_progress.emit(70+((double)j)*(10.0/POS_DIMS),"Checking Dimensions");
			#endif

			v1_window.width = sizes[j];
			v1_window.height = sizes[j+1];
			if(-1 != ioctl(fd,VIDIOCSWIN,&v1_window)) {
				#ifdef VCVIDEO_DEBUG
				cerr << "Found: " << sizes[j] << "x" << sizes[j+1] << endl;
				#endif
				capableDimensions.push_back(pair<int,int>(sizes[j],sizes[j+1]));
			}

		}
		#ifdef VCVIDEO_DEBUG
		cerr << "--------------------------------------------------\n" << endl;
		#endif

		#ifdef SIGCPP
		sig_progress.emit(80,"Setting Dimensions");
		#endif

		try {
			if(!setDimensions(v1_capabilities.maxwidth, v1_capabilities.maxheight))
				throw string ("Dimensions weren't set to desired size.");
		}
		catch (string s) {
			throw s;
		}

		#ifdef SIGCPP
		sig_progress.emit(90,"Setting Buffer");
		#endif

		setBufferSize();

		#ifdef SIGCPP
		sig_progress.emit(100,"Done");
		#endif

		//! \todo Split this function up into common chunks. Like the setBufferSize above.

	}

	/*!
		Set the buffer size for frames.  This should change on capture size change,
		capture mode change.

		\throws string If this is a V4L2 device, we don't have an implementation yet.

		\todo V4L2 implementation.
	*/
	void videoDevice::setBufferSize () {
		if(isV4L2)
			throw string("V4L2 Not supported yet.");

		bufferSize = (v1_controls.depth/8)*v1_window.height*v1_window.width;
	}

	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// Access ////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	/*!
		Get a frame from the device.

		\todo V4L2 Implementation.
		\todo Memory mapping implementation.

		\throws string If device is not initialized.
		\throws string If it is a V4L2 device.
		\throws string If camera uses an unsupported format.

		\param frame The vdFrame struct to store into.
	*/
	void videoDevice::getFrame (vdFrame & frame) {
		//! \todo Timestamp!

		if(!live)
			throw string("Device not initialized.");

		if(isV4L2) {
			throw string("V4L2 Not yet supported.");
		}
		else {
			frame.width = v1_window.width;
			frame.height = v1_window.height;
			//! \todo Set buffer size somewhere else and base it on palette.
			if(frame.bufferSize != bufferSize) {
				frame.bufferSize = bufferSize;
				if(NULL != frame.buffer)
					delete frame.buffer;
				frame.buffer = new char [frame.bufferSize];
			}
			if(-1 == read(fd, frame.buffer, frame.bufferSize))
				throw string("Failed to read data from device.");
			switch (v1_controls.palette) {
				case VIDEO_PALETTE_RGB24:
					fmt_VIDEO_PALETTE_RGB24(frame);
					break;
				//! \todo The rest of these formats...
				case VIDEO_PALETTE_GREY:
				case VIDEO_PALETTE_HI240:
				case VIDEO_PALETTE_RGB565:
				case VIDEO_PALETTE_RGB555:
				case VIDEO_PALETTE_RGB32:
				case VIDEO_PALETTE_YUV422:
				case VIDEO_PALETTE_YUYV:
				case VIDEO_PALETTE_UYVY:
				case VIDEO_PALETTE_YUV420:
				case VIDEO_PALETTE_YUV411:
				case VIDEO_PALETTE_RAW:
				case VIDEO_PALETTE_YUV422P:
				case VIDEO_PALETTE_YUV411P:
				default:
					throw string("Frame is in an unsupported format: "+v1_paletteName(v1_controls.palette));
					break;
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Controls ///////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////


	/////////////////////////////// INTEGER CONTROLS ////////////////////////////
	/*!
		Gets whether the control is used on the device.

		\throws string If device not started.
		\throws string If an invalid control is specified.

		\param controlType The type of control to check.
		\return True if used.
	*/
	bool videoDevice::getControlSupported(const vdIntegerControl controlType) {
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
		Get the current setting of a control, if possible.

		\throws string If device not started.
		\throws string If an invalid control is specified.
		\throws string If that control is not used on this device.
		\throws string If it can't retrieve the value.

		\param controlType The type of control to get.
		\return The integer control value.
	*/
	int videoDevice::getControlValue (const vdIntegerControl controlType) {
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
						throw string("Saturation control not used on this device.");
					get.id = V4L2_CID_SATURATION;
					break;
				case HUE:
					if(hue.flags & V4L2_CTRL_FLAG_DISABLED)
						throw string("Hue control not used on this device.");
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
					if(!getControlSupported(HUE))
						throw string("Hue control not used on this device.");
					return v1_controls.hue;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the minimum setting of a control, if possible.

		\throws string If device not started.
		\throws string If an invalid control is specified.
		\throws string If that control is not used on this device.
		\throws string If it can't retrieve the value.

		\param controlType The type of control to get.
		\return The minimum value.
	*/
	int videoDevice::getControlMinimum (const vdIntegerControl controlType) {
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
					if(!getControlSupported(HUE))
						throw string("Hue control not used on this device.");
					return 0;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the maximum setting of a control, if possible.

		\throws string If device not started.
		\throws string If an invalid control is specified.
		\throws string If that control is not used on this device.
		\throws string If it can't retrieve the value.

		\param controlType The type of control to get.
		\return The maximum value.
	*/
	int videoDevice::getControlMaximum (const vdIntegerControl controlType) {
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
					if(!getControlSupported(HUE))
						throw string("Hue control not used on this device.");
					return 65535;
				default:
					throw string("Invalid integer control type.");
					break;
			}
		}
	}

	/*!
		Get the minimum effective interval for a control, if possible.

		\throws string If device not started.
		\throws string If an invalid control is specified.
		\throws string If that control is not used on this device.
		\throws string If it can't retrieve the value.

		\param controlType The type of control to get.
		\return The minimum effective interval for the control value.
	*/
	int videoDevice::getControlStep (const vdIntegerControl controlType) {
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
		Sets a control on the device, if possible.

		\throws string If device not started.
		\throws string If an invalid control is specified.
		\throws string If that control is not used on this device.
		\throws string If it can't setthe value.

		\param controlType The control to set.
		\param value The new brightness value.
	*/
	void videoDevice::setControlValue (const vdIntegerControl controlType, const int value) {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2) {
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
		else {
			switch(controlType) {
				case BRIGHTNESS:
					v1_controls.brightness = value;
					break;
				case CONTRAST:
					v1_controls.contrast = value;
					break;
				case HUE:
					v1_controls.hue = value;
					break;
				case SATURATION:
					throw string("V4L1 does not support saturation.");
					break;
				default:
					throw string("Invalid integer control.");
					break;
			}
			if(-1 == ioctl(fd,VIDIOCSPICT,&v1_controls))
				throw string("Can't set properties.");
			if(-1 == ioctl(fd,VIDIOCGPICT,&v1_controls))
				throw string("Can't get properties.");
		}

	}

	/*!
		Get what a control represents as a "string".

		\throws string On invalid control type.

		\param control The control to get a name for.
		\return The name of the control.
	*/
	string videoDevice::getControlString (const vdIntegerControl control) {
		switch(control) {
			case BRIGHTNESS: return "Brightness";
			case CONTRAST: return "Contrast";
			case SATURATION: return "Saturation";
			case HUE: return "Hue";
			default: throw string ("Invalid vdIntegerControl type.");
		}
	}

	/*!
		Gets all integer controls that "should" work on this device.
		No promises though.

		\throws string If device not initialized.
		\throws string If V4L2 - Not yet supported.

		\todo Add V4L2 support.

		\return A vector of control types.
	*/
	vector <vdIntegerControl> videoDevice::getSupportedIntegerControls () {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2)
			throw string("V4L2 Not supported yet.");

		vector <vdIntegerControl> toReturn;

		try { if(getControlSupported(BRIGHTNESS)) toReturn.push_back(BRIGHTNESS); } catch (string s) {}
		try { if(getControlSupported(CONTRAST)) toReturn.push_back(CONTRAST); } catch (string s) {}
		try { if(getControlSupported(SATURATION)) toReturn.push_back(SATURATION); } catch (string s) {}
		try { if(getControlSupported(HUE)) toReturn.push_back(HUE); } catch (string s) {}

		return toReturn;
	}
	/////////////////////////////// END INTEGER CONTROLS ////////////////////////

	////////////////////////////////// DOUBLE CONTROLS //////////////////////////
	bool videoDevice::getControlSupported (const vdDoubleControl) {
		if(!live)
				throw string("Device is not initialized.");

		return false;
	}

	double videoDevice::getControlValue (const vdDoubleControl) {
		if(!live)
			throw string("Device is not initialized.");

		return 0.0;
	}

	double videoDevice::getControlMinimum (const vdDoubleControl) {
		if(!live)
			throw string("Device is not initialized.");

		return 0.0;
	}

	double videoDevice::getControlMaximum (const vdDoubleControl) {
		if(!live)
			throw string("Device is not initialized.");

		return 0.0;
	}

	double videoDevice::getControlStep (const vdDoubleControl) {
		if(!live)
			throw string("Device is not initialized.");

		return 0.0;
	}

	void videoDevice::setControlValue (const vdDoubleControl, const double) {
		if(!live)
			throw string("Device is not initialized.");
	}

	string videoDevice::getControlString(const vdDoubleControl control) {
		return "UNSUPPORTED";
	}

	/*!
		Gets all double controls that "should" work on this device.
		No promises though.

		\throws string If device not initialized.
		\throws string If V4L2 - Not yet supported.

		\todo Add V4L2 support.

		\return A vector of control types.
	*/
	vector <vdDoubleControl> videoDevice::getSupportedDoubleControls() {
		if(!live)
			throw string("Device is not initialized.");

		if(isV4L2)
			throw string("V4L2 Not supported yet.");

		vector <vdDoubleControl> toReturn;

		return toReturn;
	}
	/////////////////////////////// END DOUBLE CONTROLS /////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// Other /////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
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

	string videoDevice::v1_paletteName (int p) {
		switch (p) {
			case VIDEO_PALETTE_GREY: return "VIDEO_PALETTE_GREY";
			case VIDEO_PALETTE_HI240: return "VIDEO_PALETTE_HI240";
			case VIDEO_PALETTE_RGB565: return "VIDEO_PALETTE_RGB565";
			case VIDEO_PALETTE_RGB555: return "VIDEO_PALETTE_RGB555";
			case VIDEO_PALETTE_RGB24: return "VIDEO_PALETTE_RGB24";
			case VIDEO_PALETTE_RGB32: return "VIDEO_PALETTE_RGB32";
			case VIDEO_PALETTE_YUV422: return "VIDEO_PALETTE_YUV422";
			case VIDEO_PALETTE_YUYV: return "VIDEO_PALETTE_YUYV";
			case VIDEO_PALETTE_UYVY: return "VIDEO_PALETTE_UYVY";
			case VIDEO_PALETTE_YUV420: return "VIDEO_PALETTE_YUV420";
			case VIDEO_PALETTE_YUV411: return "VIDEO_PALETTE_YUV411";
			case VIDEO_PALETTE_RAW: return "VIDEO_PALETTE_RAW";
			case VIDEO_PALETTE_YUV422P: return "VIDEO_PALETTE_YUV422P";
			case VIDEO_PALETTE_YUV411P: return "VIDEO_PALETTE_YUV411P";
			default: return "Invalid V4L1 Palette";
		}
	}

	/*!
		Attempts to set the device to the specified dimensions.

		\throws string If V4L2 device.

		\param width The width to set.
		\param height The height to set.

		\return True if set exactly to those values. False otherwise.
	*/
	bool videoDevice::setDimensions (unsigned int width, unsigned int height) {
		if(isV4L2) {
			throw string("V4L2 Not supported yet.");
		}
		else {
			// Default with the biggest size capture
			v1_window.width = width;
			v1_window.height = height;
			if(-1 == ioctl(fd,VIDIOCSWIN,&v1_window))
				return false;
			if(-1 == ioctl(fd,VIDIOCGWIN,&v1_window))
				return false;

			return (v1_window.width == width && v1_window.height == height);
		}
	}

	/*!
		Get the dimensions that the device is capable of capturing in.

		\throws string If device is not initialized.

		\return A vector of pairs of dimensions, pair[0] is width, pair[1] is height.
	*/
	vector < pair <int,int> > videoDevice::getValidDimensions () {
		if(!live)
			throw string("Device is not initialized.");

		return capableDimensions;
	}

	/////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Formats ////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	/*!
		Convert the palette to the default format.

		\todo Performance check.

		\param frame The frame to operate on. Must be filled correctly.
	*/
	void videoDevice::fmt_VIDEO_PALETTE_RGB24(vdFrame & frame) {
		char temp;
		for(unsigned long i = 0; i+2 < frame.bufferSize; i += 3) {
			 temp = frame.buffer[i];
			 frame.buffer[i] = frame.buffer[i+2];
			 frame.buffer[i+2] = temp;
		}
	}


	/////////////////////////////////////////////////////////////////////////////
	///////////////////////////////// Static ////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	/*!
		Attempts to enumerate the devices attached to the system.
		Checks for devices named "video#" in the /dev directory.

		\note These are not guaranteed to be legitimate or compatible devices, it's best guess.

		\throws string When it cannot open the /dev directory.

		\return A vector of strings containing the absolute path to the device.
	*/
	vector <string> videoDevice::enumerateDevices () {
		vector <string> returnValue;
		DIR *dp;
		struct dirent *ep;

		dp = opendir ("/dev/");
		if(dp != NULL) {
			ep = readdir(dp);
			while(ep != NULL) {
				if(0 == strncmp(ep->d_name,"videoX",5))
					returnValue.push_back(string("/dev/")+ep->d_name);
				ep = readdir(dp);
			}
			(void) closedir (dp);
		}
		else
			throw string ("Could not open /dev/ directory.");

		return returnValue;
	}
}
