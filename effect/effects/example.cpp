#include <string>
#include <vector>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

extern "C" std::string effect_name () { return "Example Effect"; }
extern "C" std::string effect_description () { return "This is an example libvcvideo plugin."; }
extern "C" double effect_version () { return 1.01; }
extern "C" bool effect_init (vc::vdFrame & initFrame) { /* Nothing to init. */ return true; }
extern "C" bool effect_deinit () { /* Nothing to deinit. */ return true; }
extern "C" void effect_apply (vc::vdFrame & applyTo, std::vector<effectArgument> args) {
	// Parse args and do work here.
}
