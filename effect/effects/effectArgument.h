#ifndef EFFECTARGUMENT_H
#define EFFECTARGUMENT_H

#include <string>

enum effectArgumentType {
	BOOLEAN,
	INTEGER,
	DOUBLE,
	STRING
};

struct effectArgument {
	std::string name;
	std::string description;
	effectArgumentType type;
	bool _boolean;
	int _integer;
	double _double;
	std::string _string;
	effectArgument() : _boolean(true), _integer(0), _double(0.0), _string("") {};
	effectArgument(std::string _name, std::string _description, effectArgumentType _type)
	: name(_name), description(_description), type(_type), _boolean(true), _integer(0), _double(0.0), _string("") {}
};

#endif
