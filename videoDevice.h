#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

namespace vc {

	class videoDevice {

		public:
			videoDevice(string);
			~videoDevice();

			bool open();

			string getCardName();

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
	};

}

#endif
