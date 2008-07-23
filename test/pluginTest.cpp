#include <iostream>
#include "effect/effects.h"

int main (int argc, char ** argv) {

	if(argc < 2) {
		std::cerr << "You must provide paths to plugins to be tested." << std::endl;
		return -1;
	}

	vc::effects * eff = vc::effects::instance();

	for(int i = 1; i < argc; ++i)
		eff->registerEffect(argv[i]);

	std::cout << eff->toString() << std::endl;

}
