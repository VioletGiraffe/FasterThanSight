#pragma once

#include "bookmarks/cbookmark.h"
#include "reader/creader.h"
#include "recentFiles/crecentfileslist.h"

DISABLE_COMPILER_WARNINGS
#include <QObject>
RESTORE_COMPILER_WARNINGS

#include <deque>

class CController : public QObject, private CReader::ReaderInterface
{
	Q_OBJECT

public:
	CController();

	void setShowPivot(bool show);

	void loadBookmarksFromSettings();
	void saveBookmarksToSettings() const;

	std::deque<CBookmark>& bookmarks();

private:
	// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void updateInfo() override;
	void stateChanged(const CReader::State newState) override;

signals:
	void onDisplayUpdateRequired(QString text, bool showPivot, int pivotCharacterIndex);

	void onChapterProgressUpdated(int progressPercentage, QString chapterProgressDescription);
	void onGlobalProgressDescriptionUpdated(QString progressDescription);

	void onReaderStateChanged(CReader::State state);

private:
	// Reader
	CReader _reader;
	bool _showPivot = false;

	// Bookmarks
	std::deque<CBookmark> _bookmarks;
	CRecentFilesList _recentFiles;
};
