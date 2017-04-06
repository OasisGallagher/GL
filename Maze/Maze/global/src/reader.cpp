#include <fstream>

#include "debug.h"
#include "reader.h"

bool Reader::ReadAll(const std::string& file, std::string& text) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	text.clear();
	const char* seperator = "";

	std::string line;
	for (; getline(ifs, line);) {
		text += seperator;
		text += line;
		seperator = "\n";
	}

	ifs.close();

	return true;
}

