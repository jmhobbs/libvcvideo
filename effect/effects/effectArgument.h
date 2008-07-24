#ifndef EFFECTARGUMENT_H
#define EFFECTARGUMENT_H

#include <string>

namespace vc {
	//! Possible types of arguments.
	enum effectArgumentType {
		BOOLEAN,
		INTEGER,
		DOUBLE,
		STRING
	};

	//! A container for passing arguments into plugin effect functions.
	struct effectArgument {
		//! The name of the argument. Required.
		std::string name;
		//! A brief description of the argument.
		std::string description;
		//! The type of the argument.
		effectArgumentType type;
		//! The value of the argument if it is a boolean.
		bool _boolean;
		//! The value of the argument if it is an integer.
		int _integer;
		//! The value of the argument if it is a double.
		double _double;
		//! The value of the argument if it is a string.
		std::string _string;
		effectArgument() : _boolean(true), _integer(0), _double(0.0), _string("") {};
		effectArgument(std::string _name, std::string _description, effectArgumentType _type)
		: name(_name), description(_description), type(_type), _boolean(true), _integer(0), _double(0.0), _string("") {}
	};
}
#endif
