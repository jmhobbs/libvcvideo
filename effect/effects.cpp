#include "effects.h"

namespace vc {

	effects * effects::pinstance = NULL;

	effects * effects::instance () {
		if(pinstance == NULL)
			pinstance = new effects();
		return pinstance;
	}

	effects::effects () { /* Nothing to do... */ }

	void effects::populateRegistry () {
		// TODO: Look in default plugin path and load up plugins
	}

	void effects::registerEffect (std::string filename) {

		effect tempEffect;

		GModule * module;
		module = g_module_open(filename.c_str(), G_MODULE_BIND_LAZY);
		if (!module)
			throw std::string("Cannot load plugin: "+std::string(g_module_error()));

		// Load symbols...
		if (!g_module_symbol(module, "effect_name", (gpointer *)&(tempEffect.name))) {
			g_module_close(module);
			throw std::string("Cannot load effect_get symbol.");
		}

		// Check symbols...
		if (tempEffect.name == NULL) {
			g_module_close(module);
			throw std::string("Name symbol is NULL.");
		}
		g_module_make_resident(module);

		registeredEffects.insert(std::pair<std::string,effect>(tempEffect.name(),tempEffect));

	}

	std::string effects::toString () {
		std::string effectsReg("Registered effects by name: ");
 		for(std::map<std::string,effect>::iterator it = registeredEffects.begin() ; it != registeredEffects.end(); it++ )
    	effectsReg += "'"+it->first+"' ";
		return effectsReg;
	}

}
