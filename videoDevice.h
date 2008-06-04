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
#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <string>

using std::string;

namespace vc {

	enum vdIntegerControl {
		BRIGHTNESS,
		CONTRAST,
		SATURATION,
		HUE
	};

	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			void init();

			string getCardName();

			// Controls
			int getIntegerControlValue(const vdIntegerControl);
			int getIntegerControlMinimum(const vdIntegerControl);
			int getIntegerControlMaximum(const vdIntegerControl);
			int getIntegerControlStep(const vdIntegerControl);
			void setIntegerControlValue(const vdIntegerControl, const int);

		private:

			void v2_init();
			void v1_init();

			string deviceName;
			bool live; // Penultimate determinant of device status
			int fd;
			bool isV4L2; // If true, use v4l2 stuff.

			v4l2_capability v2_capabilities;
			video_capability v1_capabilities;

			// 6 is an arbitrary magic number
			#define MAX_INPUTS 6
			v4l2_input v2_inputs[MAX_INPUTS];
			video_channel v1_inputs[MAX_INPUTS];
			int inputCount;
			int currentInput;

			// Control capabilities flags
			//! \todo Add more controls
			v4l2_queryctrl brightness;
			v4l2_queryctrl contrast;
			v4l2_queryctrl saturation;
			v4l2_queryctrl hue;
			video_picture v1_controls;
	};

};

#endif
