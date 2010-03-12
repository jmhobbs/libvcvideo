# What is it?

libvcvideo is targeted as a super-simple cross platform video device library. It's starting with the most basic features and building up, all while keeping a simple API and strong documentation for beginners.

Currently only select devices are supported and it is in pre-release condition. It only has compatibility with webcam devices that use the original V4L specifications and output RGB24 data. This is actually a very large set of devices, as that is the generic description for most spca5xx and gspca devices. This is limited because that is the only type of device I posses at this time, more will be added as I can find devices or the time to research them.

Additionally, libvcvideo has plugin based effects support and a small sample of effects are included with the install.

Though incomplete, if you have the right device it is super simple. Counting variable declaration you only need four lines of library code to get a frame from a camera, as the basic example shows below.

## Brief Example

	#include <iostream>
	using std::cerr;
	using std::endl;
	#include <string>
	using std::string;

	#include <libvcvideo/videoDevice.h>

	int main (int argc, char ** argv) {
		vc::videoDevice device ("/dev/video0");
		vc::vdFrame frame;

		try {
			device.init();
		}
		catch(string s) {
			cerr << "Device initialization failed: " << s << endl;
			exit(1);
		}

		try {
			device.getFrame(frame);
		}
		catch(string s) {
			cout << "Failed to get frame: " << s << endl;
			exit(1);
		}
	}

That has error checking in it, but you could strip it down to nine lines...

## Really, Really Brief Example

	#include <string>
	using std::string;
	#include <libvcvideo/videoDevice.h>
	int main (int argc, char ** argv) {
		vc::videoDevice device ("/dev/video0");
		vc::vdFrame frame;
		device.init();
		device.getFrame(frame);
	}

## Documentation

Get it at <http://static.velvetcache.org/projects/libvcvideo/doc/active/>

## Dependencies

* A modern kernel with V4L.
* A compatible video device.
* glib (for effects, need gmodule)
* libsigc++ (optional)
* libmagick++ (optional, for example programs)
* gtkmm (optional, for example programs)

## Compilation

To compile a program, use the following flags:

* Compile Flags:  <tt>-I/usr/include/libvcvideo</tt>
* Linker Flags:   <tt>-lvcvideo `pkg-config gmodule-2.0 --libs`</tt>

If using effects, you need these in additional flags.

* Compile Flags:  <tt>`pkg-config gmodule-2.0 --cflags`</tt>

Lastly, if your version was compiled with libsigc++ you need this flag:

* Compile Flags:  <tt>-DSIGCPP</tt>

## Credits

This library would not exist without the wonderful documentation of V4L2

* <http://linuxtv.org/v4lwiki/index.php/Main_Page>

And the mediocre documentation of V4L :-)

* <http://www.linuxtv.org/downloads/video4linux/API/V4L1_API.html>

I was also inspired through the use of OpenCV

* <http://opencvlibrary.sourceforge.net/>