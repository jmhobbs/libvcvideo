#include "effects.h"

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

namespace vc {

	effects * effects::pinstance = NULL;

	/*!
		Because the effects class is a singleton, it must be accessed with a pointer
		and not a "real" object. Use this class to get a reference to the object.
	*/
	effects * effects::instance () {
		if(pinstance == NULL)
			pinstance = new effects();
		return pinstance;
	}

	effects::effects () { /* Nothing to do... */ }

	/*!
		This method searches the default plugin path for effects and loads them.

		\todo Recursive search.
		\todo Better test of file?

		\return The number of effects added to the registry.
	*/
	int effects::populateRegistry () {
		DIR *dp;
		struct dirent *ep;
		int effectsAdded = 0;
		#define EFFECT_INSTALL_PATH "/usr/share/libvcvideo/effects/"
		dp = opendir (EFFECT_INSTALL_PATH); //! \todo Real install path!
		if(dp != NULL) {
			ep = readdir(dp);
			int len;
			while(ep != NULL) {
				len = strlen(ep->d_name);
				if('o' == ep->d_name[len-1] && 's' == ep->d_name[len-2]) {
					try{
						registerEffect(std::string(EFFECT_INSTALL_PATH)+ep->d_name);
						++effectsAdded;
					}
					catch (std::string s) {
						//! \todo Do anything with that error?
					}
				}
				ep = readdir(dp);
			}
			(void) closedir (dp);
		}
		else
			throw std::string ("Could not open EFFECT_INSTALL_PATH directory.");

		return effectsAdded;
	}

	/*!
		This method attempts to register a given effect.

		\throws string On any failure to load the effect.

		\param filename The path to the plugin.
	*/
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

	/*!
		Gets a listing of all registered effects by name.

		\return An std::vector of std::strings with the effect names in them.
	*/
	std::vector<std::string> effects::getEffectNames () {
		std::vector<std::string> returnValue;
 		for(std::map<std::string,effect>::iterator it = registeredEffects.begin() ; it != registeredEffects.end(); it++ )
    	returnValue.push_back(it->first);
		return returnValue;
	}

	/*!
		Attempts to get an effects description.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return The description of the effect.
	*/
	std::string effects::getEffectDescription (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.description();
	}

	/*!
		Attempts to get an effects version.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return The version of the effect.
	*/
	double effects::getEffectVersion (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.version();
	}

	/*!
		Attempts to get an effects author.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return The author of the effect.
	*/
	std::string effects::getEffectAuthor (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.author();
	}

	/*!
		Attempts to get an effects contact address.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return The contact address of the effect.
	*/
	std::string effects::getEffectContact (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.contact();
	}

	/*!
		Attempts to get an effects website.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return The website of the effect.
	*/
	std::string effects::getEffectWebsite (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.website();
	}

	/*!
		Applys an effect to a frame without arguments.

		\throws string Pass through from fully specified version.

		\param _name The name of the effect.
		\param _frame The frame to work on.
	*/
	void effects::applyEffect (std::string _name, vc::vdFrame & _frame) {
		std::vector<effectArgument> temp;
		try { applyEffect(_name, _frame, temp); }
		catch (std::string s) { throw s; }
	}

	/*!
		Applys an effect to a frame.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\param _frame The frame to work on.
		\param args The argument list to use.
	*/
	void effects::applyEffect (std::string _name, vc::vdFrame & _frame, std::vector<effectArgument> args) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		it->second.apply(_frame, args);
	}

	/*!
		Get's a listing of arguments for an effect.

		\throws string If effect not registered.

		\param _name The name of the effect.
		\return A std::vector of effectArgument structs. The name, description and type fields will be filled.
	*/
	std::vector<effectArgument> effects::getArguments (std::string _name) {
		std::map<std::string,effect>::iterator it = registeredEffects.find(_name);
		if(registeredEffects.end() == it)
			throw std::string ("Effect not found.");
		return it->second.arguments();
	}
}
