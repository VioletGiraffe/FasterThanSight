#pragma once
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class CFileDecoder
{
public:
	static QString readDataAndDecodeText(const QString& filePath);
};
