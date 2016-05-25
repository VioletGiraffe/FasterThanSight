#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

struct CBookmark
{
	explicit CBookmark(const QString& file = QString(), size_t word = 0);

	QString filePath;
	size_t wordIndex = 0;
};
