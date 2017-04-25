#include "string_helper.hpp"


string stringify(double input) {
	ostringstream strs;
	strs << input;
	return strs.str();
}
