#ifndef EFFECTARGUMENT_H
#define EFFECTARGUMENT_H

#include <string>

typedef enum effectArgumentType {
	BOOLEAN,
	INTEGER,
	DOUBLE,
	STRING
};

typedef struct effectArgument {
	std::string name;
	effectArgumentType type;
	bool _boolean;
	int _integer;
	double _double;
	std::string _string;
	bool required;
};

#endif
