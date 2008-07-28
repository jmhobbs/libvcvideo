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
	This is a simple test program, dynamically linked.
	It opens up a device (you can specify which on the command line) and grabs
	one frame.
*/
#include <iostream>
#include <string>

#include <libvcvideo/videoDevice.h>
#include <libvcvideo/effects.h>

int main (int argc, char ** argv) {

	std::string devName("/dev/video0");

	if(argc > 1) {
		devName = argv[argc-1];
	}

	vc::videoDevice test (devName);

	vc::effects * fx = vc::effects::instance();
	std::cout << "Found " << fx->populateRegistry() << " effects." << std::endl;

	try {
		test.init();
	}
	catch(string s) {
		std::cout << "Init Failed: " << s << std::endl;
		return 1;
	}

	std::cout << "Card name: " << test.getCardName() << std::endl;

	vc::vdFrame tFrame;

	try {
		test.getFrame(tFrame);
		std::cout << "Got frame:" << std::endl;
		std::cout << "\tWidth:       " << tFrame.width << std::endl;
		std::cout << "\tHeight:      " << tFrame.height << std::endl;
		std::cout << "\tBuffer size: " << tFrame.bufferSize << std::endl;
	}
	catch(string s) {
		std::cout << "Failed to get frame: " << s << std::endl;
		return 1;
	}

	return 0;
}
