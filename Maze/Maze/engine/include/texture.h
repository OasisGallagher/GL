#pragma once
#include <string>

class Texture {
private:
	unsigned char* LoadBmpData(const std::string& bmp);
};