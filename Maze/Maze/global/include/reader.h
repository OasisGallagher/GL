#pragma once
#include <string>

class Reader {
public:
	static bool ReadAll(const std::string& file, std::string& text);
};
