#pragma once

#include "logger/cloggerinmemory.h"

inline CLoggerInterface& loggerInstance()
{
	using LoggerType = CLoggerInMemory;

	return loggerInstance<LoggerType>();
}
