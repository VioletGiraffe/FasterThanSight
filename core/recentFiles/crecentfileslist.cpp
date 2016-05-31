#include "crecentfileslist.h"
#include "settings/csettings.h"

DISABLE_COMPILER_WARNINGS
#include <QStringList>
DISABLE_COMPILER_WARNINGS

#define RECENT_FILES_LIST_SETTING QStringLiteral("RecentFiles/RecentFilesList")

CRecentFilesList::CRecentFilesList(size_t maxNumEntries) : _maxNumEntries(maxNumEntries)
{
	const QStringList entriesList = CSettings().value(RECENT_FILES_LIST_SETTING).toStringList();
	for (const QString& entry: entriesList)
		_entries.emplace_back(entry);
}

const std::deque<CBookmark>& CRecentFilesList::items() const
{
	return _entries;
}

void CRecentFilesList::updateWith(const QString &filePath, const size_t wordIndex)
{
	updateWith(CBookmark(filePath, wordIndex));
}

void CRecentFilesList::updateWith(const CBookmark& bookMark)
{
	if (bookMark.filePath.isEmpty())
		return;

	auto existingEntry = std::find_if(_entries.begin(), _entries.end(), [&bookMark](const CBookmark& item){
		return item.filePath == bookMark.filePath;
	});

	if (existingEntry != _entries.end())
	{
		// This file is already in the list; update the position and move it to front
		existingEntry->wordIndex = bookMark.wordIndex;
		std::rotate(_entries.begin(), existingEntry, existingEntry + 1);
	}
	else
	{
		_entries.emplace_front(bookMark);
		if (_entries.size() > _maxNumEntries)
			_entries.pop_back();
	}

	saveToSettings();
}

void CRecentFilesList::saveToSettings()
{
	QStringList entriesList;
	for (const auto& entry : _entries)
		entriesList.push_back(entry.toString());

	CSettings().setValue(RECENT_FILES_LIST_SETTING, entriesList);
}
