#include "creader.h"
#include "../parser/cfiledecoder.h"
#include "../parser/ctextparser.h"
#include "settings/csettings.h"
#include "settings.h"
#include "assert/advanced_assert.h"
#include "cpausehandler.h"

#include <math.h>

#include <algorithm>
#include <map>
#include <iterator>

DISABLE_COMPILER_WARNINGS
#include <QDebug>
#include <QTime>
RESTORE_COMPILER_WARNINGS

CReader::CReader(ReaderInterface* interface) : _interface(interface)
{
	assert_r(_interface);

	_readingTimer.setSingleShot(true);
	QObject::connect(&_readingTimer, &QTimer::timeout, [this](){
		readNextFragment();
	});

	_speedWpm = CSettings().value(READER_READING_SPEED_SETTING, READER_READING_SPEED_DEFAULT).toUInt();
}

void CReader::load(const CStructuredText& text)
{
	resetAndStop();

	QTime timer;
	timer.start();
	_text = text;
	qDebug() << "Assigning the text took" << timer.elapsed() << "ms";
	timer.restart();
	updatePauseValues();
	qDebug() << "updatePauseValues took" << timer.elapsed() << "ms";

	auto stats = _text.stats();
	qDebug()
		<< "Stats:"
		<< "\n\tChapters:" << stats.chapterCount
		<< "\n\tParagraphs:" << stats.paragraphCount
		<< "\n\tWords:" << stats.wordCount

		<< "\n\tAvg. paragraphs per chapter:" << stats.avgParagrapsPerChapter
		<< "\n\tMax. paragraphs per chapter:" << stats.maxParagrapsPerChapter

		<< "\n\tAvg. words per paragraph:" << stats.avgWordsPerParagraph
		<< "\n\tMax. words per paragraph:" << stats.maxWordsPerParagraph;
}

bool CReader::loadFromFile(const QString& filePath)
{
	CTextParser parser;
	parser.setAddEmptyFragmentAfterSentence(_clearScreenAfterSentenceEnd);

	QTime timer;
	timer.start();
	load(parser.parse(CFileDecoder::readDataAndDecodeText(filePath)));
	qDebug() << "Loading the file" << filePath << "took" << QString::number(timer.elapsed() / 1000.0f, 'f', 1) << "seconds";

	if (!_text.empty())
	{
		_filePath = filePath;
		return true;
	}
	else
		return false;
}

void CReader::setClearScreenAfterSentenceEnd(bool clear)
{
	_clearScreenAfterSentenceEnd = clear;
}

void CReader::setLongWordPauseScaling(unsigned lengthThreshold, float scaleFactor)
{
	if (lengthThreshold != _longWordThreshold || scaleFactor != _longWordDelayScaleFactor)
	{
		_longWordThreshold = lengthThreshold;
		_longWordDelayScaleFactor = scaleFactor;

		updatePauseValues();
	}
}

CReader::State CReader::state() const
{
	return _state;
}

QString CReader::filePath() const
{
	return _filePath;
}

size_t CReader::position() const
{
	return _position;
}

long double CReader::progress() const
{
	const size_t numWords = _text.totalFragmentsCount();
	return numWords ? _position / (long double)numWords : 0.0;
}

size_t CReader::timeRemainingSeconds() const
{
	//const float basePausePerWordSeconds = 60.0f / _speedWpm;
	//const float theoreticalSeconds = (_textFragments.size() - _position) * basePausePerWordSeconds;
	const float actualSeconds = std::accumulate(_pauseForFragment.cbegin() + _position, _pauseForFragment.cend(), 0) / 1000.0f;

	//const size_t actualWPM = (size_t)(60 * _textFragments.size() / actualSeconds);

	return (size_t)ceilf(actualSeconds);
}

void CReader::updateInterface() const
{
	_interface->stateChanged(_state);
	_interface->updateDisplay(_position);
}

size_t CReader::totalNumWords() const
{
	return _text.totalFragmentsCount();
}

void CReader::resumeReading()
{
	_state = Reading;
	_interface->stateChanged(_state);
	readNextFragment();
}

void CReader::pauseReading()
{
	_readingTimer.stop();

	_state = Paused;
	_interface->stateChanged(_state);
}

void CReader::togglePause()
{
	if (_state == Reading)
		pauseReading();
	else
		resumeReading();
}

void CReader::resetAndStop()
{
	pauseReading();
	_position = 0;
	_interface->updateInfo();
}

void CReader::goToWord(size_t wordIndex)
{
	if (_position >= _text.totalFragmentsCount())
		return;

	_position = wordIndex;
	_interface->updateDisplay(_position);
	_interface->updateInfo();
}

void CReader::toPreviousChapter()
{
	goToWord(_text.previousChapterStartIndex(_position));
}

void CReader::toPreviousParagraph()
{
	goToWord(_text.previousParagraphStartIndex(_position));
}

void CReader::toNextParagraph()
{
	goToWord(_text.nextParagraphStartIndex(_position));
}

void CReader::toNextChapter()
{
	goToWord(_text.nextChapterStartIndex(_position));
}

size_t CReader::readingSpeed() const
{
	return _speedWpm;
}

void CReader::setReadingSpeed(size_t wpm)
{
	CSettings().setValue(READER_READING_SPEED_SETTING, (quint64)wpm);
	_speedWpm = wpm;
	updatePauseValues();
}

void CReader::readNextFragment()
{
	if (_position >= _text.totalFragmentsCount())
	{
		resetAndStop();
	}
	else if (_state == Reading)
	{
		_interface->updateDisplay(_position);
		_interface->updateInfo();
		// Queue up the next word
		_readingTimer.start(_pauseForFragment[_position]);
		++_position;
	}
}

// Calculates the pause after the specific fragment in ms
size_t CReader::pauseForFragment(const TextFragment& fragment) const
{
	const float basePause = 60e3f / _speedWpm;
	const unsigned wordLength = (unsigned)fragment.word().length();

	const float pauseFactor = CPauseHandler::instance().pauseFactorForDelimiter(fragment.delimiter());
	const float longWordFactor = wordLength > _longWordThreshold ? (wordLength - _longWordThreshold) * _longWordDelayScaleFactor : 1.0f;
	return (size_t)round(basePause * pauseFactor * longWordFactor);
}

// Recalculate all the pauses for the entire text
void CReader::updatePauseValues()
{
	_pauseForFragment.resize(_text.totalFragmentsCount());
	for (const IndexedFragment& fragment: _text)
		_pauseForFragment[fragment.fragmentIndex] = pauseForFragment(fragment.fragment);

	_interface->updateInfo();
}
