#include <iostream>
#include <vector>
#include <string>

#include "lib/effects.h"

int main (int argc, char ** argv) {

	if(argc < 2) {
		std::cerr << "You must provide paths to libvcvideo effect plugins." << std::endl;
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
	std::cout << "\n---[Registered Effects]---" << std::endl;
	for(std::vector<std::string>::iterator it = regFX.begin(); it != regFX.end(); ++it) {
		std::cout << "\nName        : " << (*it) << std::endl;
		std::cout << "Version     : " << eff->getEffectVersion(*it) << std::endl;
		std::cout << "Author      : " << eff->getEffectAuthor(*it) << std::endl;
		std::cout << "Contact     : " << eff->getEffectContact(*it) << std::endl;
		std::cout << "Website     : " << eff->getEffectWebsite(*it) << std::endl;
		std::cout << "Description : " << eff->getEffectDescription(*it) << std::endl;
		std::cout << "Arguments   : " << std::endl;
		std::vector<vc::effectArgument> args = eff->getArguments(*it);
		for(std::vector<vc::effectArgument>::iterator itt = args.begin(); itt != args.end(); ++itt) {
			std::cout << "            : Name        : " << itt->name << std::endl;
			std::cout << "            : Description : " << itt->description << std::endl;
		}
		std::cout << std::endl;
	}

}
