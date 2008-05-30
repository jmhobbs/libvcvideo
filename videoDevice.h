#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

namespace vc {

	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			bool open();

			string getCardName();

			// Controls
			int getBrightness();
			void setBrightness(const int);
			in getContrast();
			void setContrast(const int);

		private:
			string deviceName;
			bool live; // Penultimate determinant of device status
			int fd;

			v4l2_capability capabilities;

			// 6 is arbitrary magic number
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
