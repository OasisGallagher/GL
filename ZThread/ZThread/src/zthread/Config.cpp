#include "Config.h"

void ZTExceptionDefault(const std::exception& exception) {
	throw exception;
}

ZTExceptionHandler ztException = ZTExceptionDefault;
