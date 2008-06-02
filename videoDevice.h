#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include <video/v4l2.h>
#include <string>

using std::string;

namespace vc {

	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			void init();

			string getCardName();

			// Controls
			int getBrightness();
			void setBrightness(const int);
			int getContrast();
			void setContrast(const int);

		private:
			string deviceName;
			bool live; // Penultimate determinant of device status
			int fd;

			v4l2_capability capabilities;

			// 6 is an arbitrary magic number
			#define MAX_INPUTS 6
			v4l2_input inputs[MAX_INPUTS];
			int inputCount;
			int currentInput;

			// Control capabilities flags
			//! \todo Add more controls
			bool brightness;
			bool contrast;
			bool saturation;
			bool hue;
	};

}

#endif
