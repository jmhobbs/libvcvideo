#include <iostream>
#include <vector>
#include <string>

#include "lib/effects.h"

int main (int argc, char ** argv) {

	//! \todo Add options to query individual plugins as arguments.

	vc::effects * eff = vc::effects::instance();

	int loaded;
	try {
		loaded = eff->populateRegistry();
	}
	catch (std::string s) {
		std::cerr << "Error loading effects: " << s << std::endl;
		_exit(0);
	}

	std::cout << "\nRegistered " << loaded << " effects, listing:" << std::endl;

	std::vector<std::string> regFX = eff->getEffectNames();
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
