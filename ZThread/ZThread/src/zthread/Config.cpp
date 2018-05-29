#include "Config.h"
void ZTExceptionDefault(const std::exception& exception) {
	throw exception;
}
namespace ZThread {
	ZTExceptionHandler ztException = ZTExceptionDefault;
}