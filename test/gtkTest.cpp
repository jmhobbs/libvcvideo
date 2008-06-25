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

			if(vd.getIntegerControlUsed(vc::BRIGHTNESS)) {
				lblBrightness.set_text("Brightness");
				vbox.pack_start(lblBrightness,false,false,0);
				sbrightness.set_range(
					vd.getIntegerControlMinimum(vc::BRIGHTNESS),
					vd.getIntegerControlMaximum(vc::BRIGHTNESS)
				);
				sbrightness.set_digits(0);
				sbrightness.set_value(vd.getIntegerControlValue(vc::BRIGHTNESS));
				sbrightness.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeBrightness));
				vbox.pack_start(sbrightness,false,false,0);
			}

			if(vd.getIntegerControlUsed(vc::SATURATION)) {
				lblSaturation.set_text("Saturation");
				vbox.pack_start(lblSaturation,false,false,0);
				ssaturation.set_range(
					vd.getIntegerControlMinimum(vc::SATURATION),
					vd.getIntegerControlMaximum(vc::SATURATION)
				);
				ssaturation.set_digits(0);
				ssaturation.set_value(vd.getIntegerControlValue(vc::SATURATION));
				ssaturation.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeSaturation));
				vbox.pack_start(ssaturation,false,false,0);
			}

			if(vd.getIntegerControlUsed(vc::CONTRAST)) {
				lblContrast.set_text("Contrast");
				vbox.pack_start(lblContrast,false,false,0);
				scontrast.set_range(
					vd.getIntegerControlMinimum(vc::CONTRAST),
					vd.getIntegerControlMaximum(vc::CONTRAST)
				);
				scontrast.set_digits(0);
				scontrast.set_value(vd.getIntegerControlValue(vc::CONTRAST));
				//scontrast.set_update_policy(Gtk::UPDATE_DELAYED);
				scontrast.signal_change_value().connect(sigc::mem_fun(*this,&Viewer::changeContrast));
				vbox.pack_start(scontrast,false,false,0);
			}

			if(vd.getIntegerControlUsed(vc::HUE)) {
				lblHue.set_text("Hue");
				vbox.pack_start(lblHue,false,false,0);
				shue.set_range(
					vd.getIntegerControlMinimum(vc::HUE),
					vd.getIntegerControlMaximum(vc::HUE)
				);
				shue.set_digits(0);
				shue.set_value(vd.getIntegerControlValue(vc::HUE));
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
			try { vd.setIntegerControlValue(vc::BRIGHTNESS,value); }
			catch(string s) {
				std::cerr << "Can't change brightness: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeContrast(Gtk::ScrollType type, double value) {
			try { vd.setIntegerControlValue(vc::CONTRAST,value); }
			catch(string s) {
				std::cerr << "Can't change contrast: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeSaturation(Gtk::ScrollType type, double value) {
			try { vd.setIntegerControlValue(vc::SATURATION,value); }
			catch(string s) {
				std::cerr << "Can't change saturation: " << s << std::endl;
				return false;
			}
			return true;
		}

		bool changeHue(Gtk::ScrollType type, double value) {
			try { vd.setIntegerControlValue(vc::HUE,value); }
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
