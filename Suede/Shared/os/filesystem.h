#pragma once
#include <vector>
#include "../shareddefines.h"

class SHARED_API FileSystem {
public:
	static time_t GetFileLastWriteTime(const char* fileName);
	static void ListAllFiles(std::vector<std::string>& paths, const char* directory, const char* postfix = nullptr);

	static bool ReadAllText(const std::string& file, std::string& text);
	static bool ReadAllLines(const std::string& file, std::vector<std::string>& lines);

	static std::string GetFileName(const std::string& path);
	static std::string GetFileNameWithoutExtension(const std::string& path);

	static std::string GetDirectory(const std::string& path);

private:
	FileSystem();
};
