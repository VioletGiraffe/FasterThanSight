#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

struct CBookmark
{
	explicit CBookmark(const QString& serializedBookmark);
	CBookmark(const QString& path, const size_t position);

	QString toString() const;

	QString filePath;
	size_t wordIndex = 0;
};
