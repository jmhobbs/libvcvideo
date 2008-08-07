/*!
	This is an example of a libvcvideo effect.  To be valid, a plugin MUST implement
	each of the function in this file, even if it's just a dummy.
*/
#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "lib/effectArgument.h"

/*
	These are all informational except the name. The name must be unique from all
	of the plugins already loaded, or it will not get used.
*/
extern "C" std::string effect_name () { return "Example Effect"; }
extern "C" std::string effect_description () { return "This is an example libvcvideo plugin. It does nothing at all."; }
extern "C" std::string effect_author () { return "John Hobbs"; }
extern "C" std::string effect_website () { return "http://www.velvetcache.org/"; }
extern "C" std::string effect_contact () { return "john@velvetcache.org"; }
extern "C" double effect_version () { return 1.0; }
/*
	The rest of these are the real workers. If you have any global data you need
	to initialize, use the effect_init. Likewise on deinitialization.
*/
extern "C" bool effect_init (vc::vdFrame & initFrame) { return true; }
extern "C" bool effect_deinit () { return true; }
/*
	This guy does the real work. The vector is for passing arguments to your effect.
	This should be parsed as each effectArgument is a struct with a name, type, and
	value.
*/
extern "C" void effect_apply (vc::vdFrame & applyTo, std::vector<vc::effectArgument> args) {}
/*
	Each plugin is queried for it's arguments which are sent back as a vector of effectArgument structs.
	The important fields here are name, description and type.
*/
extern "C" std::vector<vc::effectArgument> effect_arguments () {
	std::vector<vc::effectArgument> ret;
	ret.push_back(vc::effectArgument("Example Argument","Doesn't do anything.",vc::BOOLEAN));
	return ret;
}
