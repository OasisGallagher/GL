#pragma once
#include <string>

class Reader {
public:
	static bool ReadAllText(const std::string& file, std::string& text);
};
