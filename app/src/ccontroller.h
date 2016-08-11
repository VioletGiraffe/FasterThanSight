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

	void setFontSize(int points);
	int fontSizePoints() const;

	const std::deque<CBookmark>& bookmarks() const;
	void addBookmark(const QString& path, const size_t position);
	// Returns true on success, false if nothing was opened
	bool bookmarkCurrentPosition();
	void updateBookmarks(const std::deque<CBookmark>& newBookmarks);

	bool openLastPosition();
	bool openBookmark(const CBookmark& bookmark);
	bool openFile(const QString& filePath, size_t position);

	const std::deque<CBookmark>& recentLocations() const;

	// Reading process control
	int readingSpeed() const;
	void setReadingSpeed(int wpm);

	void togglePause();
	void pauseReading();
	void resetAndStop();

	void toPreviousChapter();
	void toPreviousParagraph();
	void toNextParagraph();
	void toNextChapter();

	int totalNumWords() const;
	int position() const;
	void goToWord(int wordIndex);

	const CStructuredText& text() const;

	void pauseAndSaveState();

	void log(const QString& message);

private:
	void loadBookmarksFromSettings();
	void saveBookmarksToSettings() const;

	// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void updateInfo() override;
	void stateChanged(const CReader::State newState) override;

signals:
	void displayUpdateRequired(QString text, bool showPivot, int pivotCharacterIndex);
	void fontSizeChanged(int pointSize);

	void chapterProgressUpdated(int progressPercentage, QString chapterProgressDescription);
	void globalProgressDescriptionUpdated(QString progressDescription);

	void readerStateChanged(CReader::State state);
	// Can be used in QML to avoid having to register the CReader::State type
	void readerStateChangedInt(int state);

	void fileOpened(bool success, QString shortFileName);

private:
	// Reader
	CReader _reader;
	bool _showPivot = false;

	// Bookmarks
	std::deque<CBookmark> _bookmarks;
	CRecentFilesList _recentFiles;
};
