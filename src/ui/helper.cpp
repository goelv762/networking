#include "helper.hpp"
#include <cctype>

bool checkChar(char c) {
	for (const char& v : VALID) {
		if (std::tolower(c) == v) {
			return true;
		}
	}

	return false;
}
