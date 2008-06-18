#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/image.h>
#include <gtkmm/progressbar.h>

#include "lib/videoDevice.h"

class Viewer : public Gtk::Window {
	public:
		Viewer() : vd("/dev/video0") {
			loading.set_text("Initializing Device");
			add(loading);
			show_all();
			Glib::signal_timeout().connect(sigc::mem_fun(*this,&Viewer::init),100);
		}

	private:
		Gtk::ProgressBar loading;
		Gtk::Image screen;
		sigc::connection timer;
		vc::videoDevice vd;
		vc::vdFrame frame;

		bool init () {
			vd.sig_progress.connect(sigc::mem_fun(*this,&Viewer::signalCatch));
			try {
				vd.init();
			}
			catch(string s) {
				std::cerr << "Viewer::Viewer() -> " << s << std::endl;
				exit(1);
			}
			screen.set_size_request(640,480);
			remove();
			add(screen);
			show_all();
			timer = Glib::signal_timeout().connect(sigc::mem_fun(*this,&Viewer::refresh),100);
			return false;
		}

		bool refresh () {

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

			return true;
		}

		void signalCatch(int state, std::string msg) {
			loading.set_text(msg);
			loading.set_fraction(((double)state)/100.0);
			while (Gtk::Main::events_pending ())
				Gtk::Main::iteration ();
		}
};

int main (int argc, char *argv[]) {

	Gtk::Main kit (argc, argv);
	Viewer window;
	Gtk::Main::run(window);
	return 0;
}
