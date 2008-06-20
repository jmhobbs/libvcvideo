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
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#include "lib/videoDevice.h"

using namespace vc;

int main (int argc, char ** argv) {

	videoDevice test ("/dev/video0");

	vdFrame tFrames[3];
	int cFrame(0);
	char * maskBuffer = NULL;
	char * fullBuffer = NULL;
	char * finalBuffer = NULL;

	try {
		test.getFrame(tFrames[0]);
		tFrames[1] = tFrames[0];
		tFrames[2] = tFrames[0];
	}
	catch(std::string s) {
		std::cerr << "Error fetching reference frame: " << s << std::endl;
		exit(1);
	}

	int maskSize ((tFrames[0].height*tFrames[0].width)/8);
	maskBuffer = new char [maskSize];
	fullBuffer = new char [tFrames.bufferSize];

	try {
		boost::asio::io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 39394));

		for (;;) {
			tcp::socket socket(io_service);
			acceptor.accept(socket);

			try {
				test.getFrame(tFrames[++cFrame]);
				int compareFrame = (cFrame+2)%3;
				for(uint i = 0; i < tFrames[cFrame].bufferSize; i += 3) {
					// Check blue only for now, 50 is a magic number
					if(abs(tFrames[cFrame].buffer[i] - tFrames[compareFrame].buffer[i]) > 50)
						// mark the bytemask here
				}
			}
			catch(std::string s) {
				throw std::exception(s);
			}

			boost::system::error_code ignored_error;
			boost::asio::write(socket, boost::asio::buffer(message), boost::asio::transfer_all(), ignored_error);

		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	if(NULL != maskBuffer)
		delete maskBuffer;
	if(NULL != fullBuffer)
		delete fullBuffer;
	if(NULL != finalBuffer)
		delete finalBuffer;

	return 0;
}
