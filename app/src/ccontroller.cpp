#include "ccontroller.h"

#include "settings/csettings.h"
#include "uisettings.h"
#include "uihelpers.h"

CController::CController() : _reader(this)
{
	_reader.setClearScreenAfterSentenceEnd(CSettings().value(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, UI_CLEAR_SCREEN_AFTER_SENTENCE_END_DEFAULT).toBool());

	loadBookmarksFromSettings();
}

void CController::setShowPivot(bool show)
{
	_showPivot = show;
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

std::deque<CBookmark>& CController::bookmarks()
{
	return _bookmarks;
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
