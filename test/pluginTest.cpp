#include <iostream>
#include <vector>
#include <string>

#include "effect/effects.h"

int main (int argc, char ** argv) {

	if(argc < 2) {
		std::cerr << "You must provide paths to plugins to be tested." << std::endl;
		return -1;
	}

	vc::effects * eff = vc::effects::instance();

	for(int i = 1; i < argc; ++i) {
		try {
			eff->registerEffect(argv[i]);
		}
		catch (std::string s) {
			std::cerr << "Error loading " << argv[i] <<"\n\t" << s << std::endl;
		}
	}

	std::vector<std::string> regFX = eff->getEffectNames();
	std::cout << "\n---[Registered effects]---" << std::endl;
	for(std::vector<std::string>::iterator it = regFX.begin(); it != regFX.end(); ++it) {
		std::cout << "\nName        : " << (*it) << std::endl;
		std::cout << "Version     : " << eff->getEffectVersion(*it) << std::endl;
		//std::cout << "Author      : " << eff->getEffectAuthor(*it) << std::endl;
		//std::cout << "Website     : " << eff->getEffectWebsite(*it) << std::endl;
		std::cout << "Description : " << eff->getEffectDescription(*it) << std::endl;
	}

	vc::vdFrame tFrame;
	tFrame.width = 640;
	tFrame.height = 480;
	tFrame.bufferSize = 921600;
	tFrame.buffer = new char [tFrame.bufferSize];

}
