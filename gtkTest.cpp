#include <iostream>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/image.h>

#include "videoDevice.h"

class Viewer : public Gtk::Window {
	public:
		Viewer() : vd("/dev/video0") {
			add(screen);
			show_all();
			vd.sig_progress.connect(sigc::mem_fun(*this,&Viewer::signalCatch));
			try {
				vd.init();
			}
			catch(string s) {
				std::cerr << "Viewer::Viewer() -> " << s << std::endl;
				exit(1);
			}
			refresh();
		}

	private:
		Gtk::Image screen;
		sigc::connection timer;
		vc::videoDevice vd;
		vc::vdFrame frame;

		bool refresh () {

			vd.getFrame(frame);

			screen.get_window()->draw_rgb_image(
				screen.get_style()->get_bg_gc(Gtk::STATE_NORMAL),
				0,
				0,
				frame.width,
				frame.height,
				Gdk::RGB_DITHER_NONE,
				(const guchar*)frame.buffer,
				frame.width*3
			);

			while (Gtk::Main::events_pending ())
				Gtk::Main::iteration ();

			return true;
		}

		void signalCatch(int state, std::string msg) {
			std::clog << state << "/100 " << msg << std::endl;
		}
};

int main (int argc, char *argv[]) {

	Gtk::Main kit (argc, argv);
	Viewer window;
	Gtk::Main::run(window);
	return 0;
}
