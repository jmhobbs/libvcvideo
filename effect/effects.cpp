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
		//! \todo Look in default plugin path and load up plugins
	}

	void effects::registerEffect (std::string filename) {

		effect tempEffect;

		GModule * module;
		module = g_module_open(filename.c_str(), G_MODULE_BIND_LAZY);
		if (!module)
			throw std::string("Cannot load plugin: "+std::string(g_module_error()));

		// Load symbols...
		// Name is a special case, we want to check for already loaded ones.
		if (!g_module_symbol(module, "effect_name", (gpointer *)&(tempEffect.name))) {
			g_module_close(module);
			throw std::string("Cannot load effect_name symbol.");
		}
		if(registeredEffects.end() != registeredEffects.find(tempEffect.name())) {
			g_module_close(module);
			return;
		}
		// Now that we know it is new, do the rest of them.
		if (!g_module_symbol(module, "effect_description", (gpointer *)&(tempEffect.description))) {
			g_module_close(module);
			throw std::string("Cannot load effect_description symbol.");
		}
		if (!g_module_symbol(module, "effect_author", (gpointer *)&(tempEffect.author))) {
			g_module_close(module);
			throw std::string("Cannot load effect_author symbol.");
		}
		if (!g_module_symbol(module, "effect_website", (gpointer *)&(tempEffect.website))) {
			g_module_close(module);
			throw std::string("Cannot load effect_website symbol.");
		}
		if (!g_module_symbol(module, "effect_contact", (gpointer *)&(tempEffect.contact))) {
			g_module_close(module);
			throw std::string("Cannot load effect_contact symbol.");
		}
		if (!g_module_symbol(module, "effect_version", (gpointer *)&(tempEffect.version))) {
			g_module_close(module);
			throw std::string("Cannot load effect_version symbol.");
		}
		if (!g_module_symbol(module, "effect_init", (gpointer *)&(tempEffect.init))) {
			g_module_close(module);
			throw std::string("Cannot load effect_init symbol.");
		}
		if (!g_module_symbol(module, "effect_deinit", (gpointer *)&(tempEffect.deinit))) {
			g_module_close(module);
			throw std::string("Cannot load effect_deinit symbol.");
		}
		if (!g_module_symbol(module, "effect_apply", (gpointer *)&(tempEffect.apply))) {
			g_module_close(module);
			throw std::string("Cannot load effect_apply symbol.");
		}
		if (!g_module_symbol(module, "effect_arguments", (gpointer *)&(tempEffect.arguments))) {
			g_module_close(module);
			throw std::string("Cannot load effect_arguments symbol.");
		}

		// Check symbols...
		if (tempEffect.name == NULL) {
			g_module_close(module);
			throw std::string("Name symbol is NULL.");
		}
		if (tempEffect.name == NULL) {
			g_module_close(module);
			throw std::string("Description symbol is NULL.");
		}
		if (tempEffect.version == NULL) {
			g_module_close(module);
			throw std::string("Version symbol is NULL.");
		}
		if (tempEffect.init == NULL) {
			g_module_close(module);
			throw std::string("Init symbol is NULL.");
		}
		if (tempEffect.deinit == NULL) {
			g_module_close(module);
			throw std::string("Deinit symbol is NULL.");
		}
		if (tempEffect.apply == NULL) {
			g_module_close(module);
			throw std::string("Apply symbol is NULL.");
		}
		if (tempEffect.arguments == NULL) {
			g_module_close(module);
			throw std::string("Arguments symbol is NULL.");
		}

		registeredEffects.insert(std::pair<std::string,effect>(tempEffect.name(),tempEffect));
		g_module_make_resident(module);

	}

	std::vector<std::string> effects::getEffectNames () {
		std::vector<std::string> returnValue;
 		for(std::map<std::string,effect>::iterator it = registeredEffects.begin() ; it != registeredEffects.end(); it++ )
    	returnValue.push_back(it->first);
		return returnValue;
	}

	std::string effects::getEffectDescription (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.description();
	}

	double effects::getEffectVersion (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.version();
	}

	std::string effects::getEffectAuthor (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.author();
	}

	std::string effects::getEffectContact (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.contact();
	}

	std::string effects::getEffectWebsite (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.website();
	}

	void effects::applyEffect (std::string _name, vc::vdFrame & _frame) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		std::vector<effectArgument> temp;
		it->second.apply(_frame, temp);
	}

	std::vector<effectArgument> effects::getArguments (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.arguments();
	}
}
