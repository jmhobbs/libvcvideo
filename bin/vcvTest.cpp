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

/*
	This is a simple test program.
	It opens up a device (you can specify which on the command line) and grabs
	one frame.  If you built it with libmagick++ it will save it as a jpeg.
	Otherwise it just save the raw data.
*/
#include <iostream>
#include <string>
#include <vector>

#ifdef HAVE_MAGICK
#include <Magick++.h>
#else
#include <fstream>
#endif

using namespace std;

#include "lib/videoDevice.h"

using namespace vc;

int main (int argc, char ** argv) {

	string devName("/dev/video0");

	if(argc > 1) {
		devName = argv[argc-1];
	}

	bool foundOurs = false;

	try {
		cout << "Checking out what video devices are attached." << endl;
		vector <string> devices(videoDevice::enumerateDevices());
		cout << "Found " << devices.size() << " devices." << endl;

		for(vector <string>::iterator it = devices.begin(); it < devices.end(); ++it) {
			cout << "\tFound Device: " << *it;
			if(*it == devName) {
				cout << " (Thats the one we'll try to use)";
				foundOurs = true;
			}
			cout << endl;
		}
	}
	catch(string s) {
		cout << "Error enumerating devices. Oh well." << endl;
	}

	if(!foundOurs)
		cout << "Couldn't find specified device " << devName << ". Don't be surprised it we fail!\n" << endl;
	else
		cout << endl;

	videoDevice test (devName);

	try {
		test.init();
	}
	catch(string s) {
		cout << "Init Failed: " << s << endl;
		return 1;
	}

	cout << "Card name: " << test.getCardName() << endl;

	try {
		cout << "Brightness (min/max/cur): " << test.getControlMinimum(BRIGHTNESS) << "/"
				 << test.getControlMaximum(BRIGHTNESS) << "/" << test.getControlValue(BRIGHTNESS) << endl;
	}
	catch(string s) {
		cout << "Brightness Information Failed: " << s << endl;
		return 1;
	}

	vdFrame tFrame;

	try {
		test.getFrame(tFrame);
		cout << "Got frame:" << endl;
		cout << "\tWidth:       " << tFrame.width << endl;
		cout << "\tHeight:      " << tFrame.height << endl;
		cout << "\tBuffer size: " << tFrame.bufferSize << endl;
		#ifdef HAVE_MAGICK
		Magick::Image image(tFrame.width, tFrame.height, "RGB", Magick::CharPixel, tFrame.buffer);
		image.magick("JPEG");
		image.write("testImage.jpg");
		cout << "Jpeg image written to testImage.jpg." << endl;
		#else
		fstream file;
		file.open("testImage.raw",fstream::out | fstream::binary);
		if(file.good()) {
			file.write(tFrame.buffer,tFrame.bufferSize);
			cout << "Raw RGB data written to testImage.raw" << endl;
		}
		file.close();
		#endif
	}
	catch(string s) {
		cout << "Failed to get frame: " << s << endl;
		return 1;
	}

	return 0;
}
