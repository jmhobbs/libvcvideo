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
#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <string>
#include <vector>

#ifdef SIGCPP
#include <sigc++/sigc++.h>
#endif

using std::string;
using std::vector;
using std::pair;

#include "lib/vdFrame.h"

namespace vc {

	//! These are all available integer type controls.
	enum vdIntegerControl {
		BRIGHTNESS,
		CONTRAST,
		SATURATION,
		HUE
	};

	//!
	enum vdDoubleControl {
		DUMMY
	};

	/*!
		The core videoDevice object.

		This devices accesses the attached hardware through the appropriate
		interfaces and simplifies interaction through a smaller, unified API.
	*/
	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			#ifdef SIGCPP
			sigc::signal <void,int,string> sig_progress;
			#endif

			void init();

			void getFrame(vdFrame &);

			string getCardName();

			bool setDimensions(unsigned int, unsigned int);
			vector < pair <int,int> > getValidDimensions();
			pair <int,int> getDimensions();

			////////////////////////// Controls //////////////////////////
			bool getControlSupported (const vdIntegerControl);
			bool getControlSupported (const vdDoubleControl);

			int getControlValue (const vdIntegerControl);
			double getControlValue (const vdDoubleControl);

			int getControlMinimum (const vdIntegerControl);
			double getControlMinimum (const vdDoubleControl);

			int getControlMaximum (const vdIntegerControl);
			double getControlMaximum (const vdDoubleControl);

			int getControlStep (const vdIntegerControl);
			double getControlStep (const vdDoubleControl);

			void setControlValue (const vdIntegerControl, const int);
			void setControlValue (const vdDoubleControl, const double);

			static string getControlString(const vdIntegerControl);
			static string getControlString(const vdDoubleControl);

			vector <vdIntegerControl> getSupportedIntegerControls();
			vector <vdDoubleControl> getSupportedDoubleControls();

			// Static helpers
			static vector <string> enumerateDevices ();

		private:

			void v2_init();
			void v1_init();

			string v1_paletteName(int);

			void setBufferSize();

			string deviceName;
			bool live; // Penultimate determinant of device status
			int fd;
			bool isV4L2; // If true, use v4l2 stuff.

			v4l2_capability v2_capabilities;

			// 6 is an arbitrary magic number
			#define MAX_INPUTS 6
			v4l2_input v2_inputs[MAX_INPUTS];
			int inputCount;
			int currentInput;

			// Control capabilities flags
			//! \todo Add more controls
			v4l2_queryctrl brightness;
			v4l2_queryctrl contrast;
			v4l2_queryctrl saturation;
			v4l2_queryctrl hue;

			// V4L1
			video_capability v1_capabilities;
			video_channel v1_inputs[MAX_INPUTS];
			video_picture v1_controls;
			video_window v1_window;

			vector < pair <int,int> > capableDimensions;
			unsigned int bufferSize;

			// Format converters
			void fmt_VIDEO_PALETTE_RGB24(vdFrame &);
	};

};

#endif
