#ifndef EFFECTS_H
#define EFFECTS_H

#include <map>
#include <string>

namespace vc {
	class effects {
		public:
			static effects * Instance ();
			static void populateRegistry ();
		protected:
			effects ();
			effects (const effects &);
			effects & operator= (const effects &);
		private:
			static effects* pinstance;

			void registerEffect ();
			void unregisterEffect ();

			std::map <std::string, effect> registeredEffects;

	};
}
//http://www.inquiry.com/techtips/cpp_pro/10min/10min0200.asp
#endif
