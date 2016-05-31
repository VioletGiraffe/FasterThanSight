#pragma once

#include "compiler/compiler_warnings_control.h"
#include "../bookmarks/cbookmark.h"

#include <deque>

class QString;

class CRecentFilesList
{
public:
	explicit CRecentFilesList(size_t maxNumEntries = 10);
	CRecentFilesList& operator=(const CRecentFilesList&) = delete;

	const std::deque<CBookmark>& items() const;

	void updateWith(const CBookmark& bookMark);
	void updateWith(const QString& filePath, const size_t wordIndex);

private:
	void saveToSettings();

private:
	const size_t _maxNumEntries;
	std::deque<CBookmark> _entries;
};
