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
	effectArgumentType type;
	bool _boolean;
	int _integer;
	double _double;
	std::string _string;
	bool required;
};

#endif
