#ifndef VDFRAME_H
#define VDFRAME_H
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

}
#endif
