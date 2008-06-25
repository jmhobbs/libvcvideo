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
#include <vector>

#ifdef SIGCPP
#include <sigc++/sigc++.h>
#endif

using std::string;
using std::vector;
using std::pair;

namespace vc {

	//! This is a single frame from the video device.
	struct vdFrame {
		//! This is the image data in 3 bytes in BGR order.
		char * buffer;
		//! Size in bytes of the image data. Should always be 3*width.
		unsigned long bufferSize;
		//! Height of this frame.
		unsigned long height;
		//! Width of this frame.
		unsigned long width;
		vdFrame() { buffer = NULL; bufferSize = 0; }
		~vdFrame() { if(NULL != buffer) delete buffer; }
	};

	/*!
		These are all available integer type controls.
	*/
	enum vdIntegerControl {
		BRIGHTNESS,
		CONTRAST,
		SATURATION,
		HUE
	};

	/*!
		\class videoDevice
		\brief The core videoDevice object.

		This devices accesses the attached hardware through the appropriate
		interfaces and simplifies interaction through a smaller, unified API.
	*/
	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			void init();

			void getFrame(vdFrame &);

			string getCardName();
			// Controls
			bool getIntegerControlUsed(const vdIntegerControl);
			int getIntegerControlValue(const vdIntegerControl);
			int getIntegerControlMinimum(const vdIntegerControl);
			int getIntegerControlMaximum(const vdIntegerControl);
			int getIntegerControlStep(const vdIntegerControl);
			void setIntegerControlValue(const vdIntegerControl, const int);
			static string getIntegerControlString(const vdIntegerControl);
			vector <vdIntegerControl> getValidIntegerControls();

			bool setDimensions(unsigned int, unsigned int);
			vector < pair <int,int> > getDimensions();

			#ifdef SIGCPP
			sigc::signal <void,int,string> sig_progress;
			#endif

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
