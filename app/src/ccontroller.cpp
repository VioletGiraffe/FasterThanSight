#include "ccontroller.h"

#include "settings/csettings.h"
#include "uisettings.h"
#include "uihelpers.h"

DISABLE_COMPILER_WARNINGS
#include <QFileInfo>
RESTORE_COMPILER_WARNINGS

CController::CController() : _reader(this)
{
	_reader.setClearScreenAfterSentenceEnd(CSettings().value(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, UI_CLEAR_SCREEN_AFTER_SENTENCE_END_DEFAULT).toBool());

	settingsChanged();
	loadBookmarksFromSettings();

	setShowPivot(CSettings().value(UI_SHOW_PIVOT_SETTING, UI_SHOW_PIVOT_DEFAULT).toBool());
}

CController::~CController()
{
	_reader.pauseReading();
	if (!_reader.filePath().isEmpty())
		_recentFiles.updateWith(_reader.filePath(), _reader.position());
}

void CController::settingsChanged()
{
	CSettings s;
	_reader.setLongWordPauseScaling(
		s.value(UI_LONG_WORD_THRESHOLD_SETTING, UI_LONG_WORD_THRESHOLD_DEFAULT).toUInt(),
		s.value(UI_LONG_WORD_DELAY_FACTOR_SETTING, UI_LONG_WORD_DELAY_FACTOR_DEFAULT).toFloat());
}

bool CController::showPivot() const
{
	return CSettings().value(UI_SHOW_PIVOT_SETTING, UI_SHOW_PIVOT_DEFAULT).toBool();
}

void CController::setShowPivot(bool show)
{
	_showPivot = show;
	CSettings().setValue(UI_SHOW_PIVOT_SETTING, show);

	if (_reader.totalNumWords() > 0)
		updateDisplay(_reader.position());
}

void CController::setClearScreenAfterSentenceEnd(bool clear)
{
	CSettings().setValue(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, clear);
	_reader.setClearScreenAfterSentenceEnd(clear);
}

const std::deque<CBookmark>& CController::bookmarks() const
{
	return _bookmarks;
}

void CController::addBookmark(const QString& path, const size_t position)
{
	_bookmarks.emplace_back(path, position);
	saveBookmarksToSettings();
}

bool CController::bookmarkCurrentPosition()
{
	if (_reader.filePath().isEmpty())
		return false;

	addBookmark(_reader.filePath(), _reader.position());
	return true;
}

void CController::updateBookmarks(const std::deque<CBookmark>& newBookmarks)
{
	_bookmarks = newBookmarks;
	saveBookmarksToSettings();
}

void CController::openBookmark(const CBookmark& bookmark)
{
	openFile(bookmark.filePath, bookmark.wordIndex);
}

void CController::openFile(const QString &filePath, size_t position)
{
	const CBookmark lastPosition(_reader.filePath(), _reader.position());
	if (!filePath.isEmpty() && _reader.loadFromFile(filePath))
	{
		// Opening a new file - store the previous one in the Recent files list
		if (!lastPosition.filePath.isEmpty())
			_recentFiles.updateWith(lastPosition);

		_reader.goToWord(position);

		CSettings().setValue(UI_OPEN_FILE_LAST_USED_DIR_SETTING, filePath);

		emit onFileOpened(true, QFileInfo(filePath).baseName());
	}
	else
		emit onFileOpened(false, QString());
}

const std::deque<CBookmark>& CController::recentLocations() const
{
	return _recentFiles.items();
}

size_t CController::readingSpeed() const
{
	return _reader.readingSpeed();
}

void CController::setReadingSpeed(size_t wpm)
{
	_reader.setReadingSpeed(wpm);
}

void CController::togglePause()
{
	_reader.togglePause();
}

void CController::pauseReading()
{
	_reader.pauseReading();
}

void CController::resetAndStop()
{
	_reader.resetAndStop();
}

void CController::toPreviousChapter()
{
	_reader.toPreviousChapter();
}

void CController::toPreviousParagraph()
{
	_reader.toPreviousParagraph();
}

void CController::toNextParagraph()
{
	_reader.toNextParagraph();
}

void CController::toNextChapter()
{
	_reader.toNextChapter();
}

size_t CController::totalNumWords() const
{
	return _reader.totalNumWords();
}

void CController::goToWord(size_t wordIndex)
{
	_reader.goToWord(wordIndex);
}

void CController::loadBookmarksFromSettings()
{
	_bookmarks.clear();
	const QStringList serializedBookmarks = CSettings().value(UI_BOOKMARKS_STORAGE).toStringList();
	for (const QString& entry : serializedBookmarks)
		_bookmarks.emplace_back(entry);
}

void CController::saveBookmarksToSettings() const
{
	QStringList serialzedBookmarks;
	for (const CBookmark& bm : _bookmarks)
		serialzedBookmarks.push_back(bm.toString());

	CSettings().setValue(UI_BOOKMARKS_STORAGE, serialzedBookmarks);
}

void CController::updateDisplay(const size_t currentTextFragmentIndex)
{
	const auto& currentFragment = _reader.textFragment(currentTextFragmentIndex);
	const int pivot = _showPivot ?
		currentFragment._textFragment.pivotLetterIndex((TextFragment::PivotCalculationMethod)CSettings().value(UI_PIVOT_CALCULATION_METHOD, UI_PIVOT_CALCULATION_METHOD_DEFAULT).toInt())
		:
		-1;

	emit onDisplayUpdateRequired(currentFragment._textFragment.text(), _showPivot, pivot);

	const auto chapterProgress = _reader.currentChapterProgress();
	const QString chapterProgressDescription = tr("%1 out of %2 words read in this chapter\n%3 remaining")
		.arg(chapterProgress.wordsRead)
		.arg(chapterProgress.totalNumWords)
		.arg(secondsToHhhMmSs(_reader.currentChapterTimeRemainingSeconds()));

	emit onChapterProgressUpdated(chapterProgress.progressPercentage(), chapterProgressDescription);
}

void CController::updateInfo()
{
	if (_reader.state() != CReader::Finished)
	{
		emit onGlobalProgressDescriptionUpdated(
			tr("Reading word %1 out of %2 total (%3%); estimated time remaining: %4")
			.arg(_reader.totalNumWords() > 0 ? _reader.position() + 1 : 0)
			.arg(_reader.totalNumWords())
			.arg(QString::number(100 * (double) _reader.progress(), 'f', 2))
			.arg(secondsToHhhMmSs(_reader.timeRemainingSeconds())));
	}
	else
	{
		emit onGlobalProgressDescriptionUpdated(tr("Reading finished"));
		emit onChapterProgressUpdated(100, tr("Reading finished"));
	}
}

void CController::stateChanged(const CReader::State newState)
{
	emit onReaderStateChanged(newState);
}
