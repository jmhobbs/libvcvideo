#ifndef VDFRAME_H
#define VDFRAME_H

#include <time.h>

namespace vc {

	//! This is a single pixel, and most likely a waste of time.
	struct pixel {
		char blue;
		char green;
		char red;
		pixel(char _blue, char _green, char _red) { blue = _blue; green = _green; red = _red; }
	};

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
		//! Frame capture time
		time_t captured;
		vdFrame() { buffer = NULL; bufferSize = 0; }
		~vdFrame() { if(NULL != buffer) delete buffer; }
		pixel getPixel (int x, int y) {
			unsigned int j = (y*(width*3))+(x*3);
			return pixel(buffer[j],buffer[j+1],buffer[j+2]);
		}
		void setPixel (int x, int y, pixel px) {
			unsigned int j = (y*(width*3))+(x*3);
			buffer[j] = px.blue;
			buffer[j+1] = px.green;
			buffer[j+2] = px.red;
		}
	};

}
#endif
