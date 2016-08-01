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
	~CController();

public slots:
	void settingsChanged();

	bool showPivot() const;
	void setShowPivot(bool show);
	void setClearScreenAfterSentenceEnd(bool clear);

	const std::deque<CBookmark>& bookmarks() const;
	void addBookmark(const QString& path, const size_t position);
	// Returns true on success, false if nothing was opened
	bool bookmarkCurrentPosition();
	void updateBookmarks(const std::deque<CBookmark>& newBookmarks);

	void openBookmark(const CBookmark& bookmark);
	void openFile(const QString& filePath, size_t position);

	const std::deque<CBookmark>& recentLocations() const;

	// Reading process control
	size_t readingSpeed() const;
	void setReadingSpeed(size_t wpm);

	void togglePause();
	void pauseReading();
	void resetAndStop();

	void toPreviousChapter();
	void toPreviousParagraph();
	void toNextParagraph();
	void toNextChapter();

	size_t totalNumWords() const;
	size_t position() const;
	void goToWord(size_t wordIndex);

	const CStructuredText& text() const;

private:
	void loadBookmarksFromSettings();
	void saveBookmarksToSettings() const;

	// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void updateInfo() override;
	void stateChanged(const CReader::State newState) override;

signals:
	void onDisplayUpdateRequired(QString text, bool showPivot, int pivotCharacterIndex);

	void onChapterProgressUpdated(int progressPercentage, QString chapterProgressDescription);
	void onGlobalProgressDescriptionUpdated(QString progressDescription);

	void onReaderStateChanged(CReader::State state);

	void onFileOpened(bool success, QString shortFileName);

private:
	// Reader
	CReader _reader;
	bool _showPivot = false;

	// Bookmarks
	std::deque<CBookmark> _bookmarks;
	CRecentFilesList _recentFiles;
};
