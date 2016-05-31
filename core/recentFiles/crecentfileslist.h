#pragma once

#include "compiler/compiler_warnings_control.h"

class QString;

class CRecentFilesList
{
public:
	explicit CRecentFilesList(size_t maxNumEntries = 10);

	void updateWith(const QString& filePath, const size_t wordIndex);

private:
	const size_t _maxNumEntries;
};
