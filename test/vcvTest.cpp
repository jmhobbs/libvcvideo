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
#include <iostream>
#include <string>
#include <vector>

#ifdef HAVE_MAGICK
#include <Magick++.h>
#endif

using namespace std;

#include "lib/videoDevice.h"

using namespace vc;

int main (int argc, char ** argv) {

	try {
		cout << "Checking out what video devices are attached." << endl;
		vector <string> devices(videoDevice::enumerateDevices());
		cout << "Found " << devices.size() << " devices." << endl;
		if(devices.size() == 0)
			exit(0);
		for(vector <string>::iterator it = devices.begin(); it < devices.end(); ++it)
			cout << "\tFound Device: " << *it << endl;
	}
	catch(string s) {
		cout << "Error enumerating devices. Oh well." << endl;
	}

	videoDevice test ("/dev/video0");

	try {
		test.init();
	}
	catch(string s) {
		cout << "Init Failed: " << s << endl;
		exit(1);
	}

	cout << "Card name: " << test.getCardName() << endl;

	try {
		cout << "Brightness (min/max/cur): " << test.getIntegerControlMinimum(BRIGHTNESS) << "/"
				 << test.getIntegerControlMaximum(BRIGHTNESS) << "/" << test.getIntegerControlValue(BRIGHTNESS) << endl;
	}
	catch(string s) {
		cout << "Brightness Information Failed: " << s << endl;
		exit(1);
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
		#endif
	}
	catch(string s) {
		cout << "Failed to get frame: " << s << endl;
		exit(1);
	}

	return 0;
}
