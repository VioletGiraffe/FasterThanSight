#pragma once

#include "logger/cloggerinmemory.h"

typedef CLoggerInMemory LoggerType;
inline CLoggerInterface& loggerInstance()
{
	return loggerInstance<LoggerType>();
}
