#ifndef EFFECTS_H
#define EFFECTS_H

#include <map>
#include <string>
#include <vector>

#include <gmodule.h>

#include "lib/vdFrame.h"
#include "effect/effects/effectArgument.h"

namespace vc {

	typedef std::string (* effect_name) ();
	typedef std::string (* effect_description) ();
	typedef std::string (* effect_author) ();
	typedef std::string (* effect_website) ();
	typedef std::string (* effect_contact) ();
	typedef double (* effect_version) ();
	typedef bool (* effect_init) (vdFrame &);
	typedef bool (* effect_deinit) ();
	typedef void (* effect_apply) (vdFrame &, std::vector<effectArgument>);

	struct effect {
		effect_name name;
		effect_description description;
		effect_author author;
		effect_website website;
		effect_contact contact;
		effect_version version;
		effect_init init;
		effect_deinit deinit;
		effect_apply apply;
	};

	class effects {
		public:
			static effects * instance ();
			void populateRegistry ();
			void apply ();
			void registerEffect (std::string);

			std::vector<std::string> getEffectNames ();

			std::string getEffectDescription (std::string);
			double getEffectVersion (std::string);
			std::string getEffectAuthor (std::string);
			std::string getEffectWebsite (std::string);
			std::string getEffectContact (std::string);

			void applyEffect (std::string, vc::vdFrame &);

		protected:
			effects ();
			effects (const effects &);
			effects & operator= (const effects &);

		private:
			static effects * pinstance;
			std::map <std::string, effect> registeredEffects;

	};
}
#endif
