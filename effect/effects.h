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
	typedef double (* effect_version) ();
	typedef bool (* effect_init) (vdFrame &);
	typedef bool (* effect_deinit) ();
	typedef void (* effect_apply) (vdFrame &, std::vector<effectArgument>);

	struct effect {
		effect_name name;
		effect_version version;
		effect_init init;
		effect_deinit deinit;
	};

	class effects {
		public:
			static effects * instance ();
			void populateRegistry ();
			void apply ();
			void registerEffect (std::string);
			std::string toString ();
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
