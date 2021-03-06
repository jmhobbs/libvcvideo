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
	This is a small example of using libvcvideo with Gtkmm.
	I'm not a Gtk whiz, so it might be a bit hard to understand :-)
*/
#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/image.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/box.h>
#include <gtkmm/scale.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/label.h>

#include "lib/videoDevice.h"

class Viewer : public Gtk::Window {
	public:
		Viewer(Glib::ustring deviceName) : vd(deviceName), running(true) {
			set_position(Gtk::WIN_POS_CENTER);
			set_title("libvcvideo - Gtk Test - "+deviceName);
			loading.set_text("Initializing Device");
			add(loading);
			show_all();
			Glib::signal_timeout().connect(sigc::mem_fun(*this,&Viewer::init),100);
			// Got to set our sentinel variable on hide or the camera won't stop
			signal_hide().connect(sigc::mem_fun(*this,&Viewer::stopCamera));
		}

	private:
		Gtk::VBox vbox;
		Gtk::HScale sbrightness, ssaturation, scontrast, shue;
		Gtk::Label lblBrightness, lblSaturation, lblContrast, lblHue;
		Gtk::ProgressBar loading;
		Gtk::Image screen;
		sigc::connection timer;
		vc::videoDevice vd;
		vc::vdFrame frame;
		bool running;

		bool init () {
			vd.sig_progress.connect(sigc::mem_fun(*this,&Viewer::signalCatch));
			try {
				vd.init();
			}
			catch(string s) {
				std::cerr << "Viewer::Viewer() -> " << s << std::endl;
				Gtk::MessageDialog md(*this,"Could not initialize device.",false,Gtk::MESSAGE_ERROR,Gtk::BUTTONS_OK,true);
				md.set_title("Error!");
				md.run();
				exit(1);
			}
			screen.set_size_request(640,480);
			remove();

			vbox.pack_start(screen,false,false,0);

			if(vd.getControlSupported(vc::BRIGHTNESS)) {
				lblBrightness.set_text("Brightness");
				vbox.pack_start(lblBrightness,false,false,0);
				sbrightness.set_range(
					vd.getControlMinimum(vc::BRIGHTNESS),
					vd.getControlMaximum(vc::BRIGHTNESS)
				);
				sbrightness.set_digits(0);
				sbrightness.set_value(vd.getControlValue(vc::BRIGHTNESS));
				sbrightness.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeBrightness));
				vbox.pack_start(sbrightness,false,false,0);
			}

			if(vd.getControlSupported(vc::SATURATION)) {
				lblSaturation.set_text("Saturation");
				vbox.pack_start(lblSaturation,false,false,0);
				ssaturation.set_range(
					vd.getControlMinimum(vc::SATURATION),
					vd.getControlMaximum(vc::SATURATION)
				);
				ssaturation.set_digits(0);
				ssaturation.set_value(vd.getControlValue(vc::SATURATION));
				ssaturation.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeSaturation));
				vbox.pack_start(ssaturation,false,false,0);
			}

			if(vd.getControlSupported(vc::CONTRAST)) {
				lblContrast.set_text("Contrast");
				vbox.pack_start(lblContrast,false,false,0);
				scontrast.set_range(
					vd.getControlMinimum(vc::CONTRAST),
					vd.getControlMaximum(vc::CONTRAST)
				);
				scontrast.set_digits(0);
				scontrast.set_value(vd.getControlValue(vc::CONTRAST));
				//scontrast.set_update_policy(Gtk::UPDATE_DELAYED);
				scontrast.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeContrast));
				vbox.pack_start(scontrast,false,false,0);
			}

			if(vd.getControlSupported(vc::HUE)) {
				lblHue.set_text("Hue");
				vbox.pack_start(lblHue,false,false,0);
				shue.set_range(
					vd.getControlMinimum(vc::HUE),
					vd.getControlMaximum(vc::HUE)
				);
				shue.set_digits(0);
				shue.set_value(vd.getControlValue(vc::HUE));
				shue.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeHue));
				vbox.pack_start(shue,false,false,0);
			}

			add(vbox);
			show_all();
			// Set up a one shot deal to start the stream after realization.
			timer = Glib::signal_timeout().connect(sigc::mem_fun(*this,&Viewer::refresh),100);
			return false;
		}

		bool refresh () {
			timer.disconnect();

			while(running) {
				vd.getFrame(frame);

				screen.get_window()->draw_rgb_image(
					screen.get_style()->get_bg_gc(Gtk::STATE_NORMAL),
					0,
					0,
					frame.width,
					frame.height,
					Gdk::RGB_DITHER_NONE,
					(const guchar*) frame.buffer,
					frame.width*3
				);

				while (Gtk::Main::events_pending ())
					Gtk::Main::iteration ();

			}

			return false;
		}

		void signalCatch(int state, std::string msg) {
			loading.set_text(msg);
			loading.set_fraction(((double)state)/100.0);
			while (Gtk::Main::events_pending ())
				Gtk::Main::iteration ();
		}

		void stopCamera () { running = false; }

		bool changeBrightness(Gtk::ScrollType type, double value) {
			try { vd.setControlValue(vc::BRIGHTNESS,value); }
			catch(string s) {
				std::cerr << "Can't change brightness: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeContrast(Gtk::ScrollType type, double value) {
			try { vd.setControlValue(vc::CONTRAST,value); }
			catch(string s) {
				std::cerr << "Can't change contrast: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeSaturation(Gtk::ScrollType type, double value) {
			try { vd.setControlValue(vc::SATURATION,value); }
			catch(string s) {
				std::cerr << "Can't change saturation: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeHue(Gtk::ScrollType type, double value) {
			try { vd.setControlValue(vc::HUE,value); }
			catch(string s) {
				std::cerr << "Can't change hue: " << s << std::endl;
				return false;
			}
			return true;
		}
};

int main (int argc, char *argv[]) {

	Gtk::Main kit (argc, argv);
	Glib::ustring deviceName("/dev/video0");
	if(argc > 1)
		deviceName = argv[argc-1];
	Viewer window(deviceName);
	Gtk::Main::run(window);
	return 0;
}
