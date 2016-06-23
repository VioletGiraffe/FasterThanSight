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

void CReader::load(const CStructuredText& textFragments)
{
	resetAndStop();

	_textFragments.clear();
	std::transform(textFragments.cbegin(), textFragments.cend(), std::back_inserter(_textFragments), [this](const TextFragment& fragment) -> TextFragmentWithPause {
		return TextFragmentWithPause{fragment, 0};
	});

	updatePauseValues();
}

bool CReader::loadFromFile(const QString& filePath)
{
	CTextParser parser;
	parser.setAddEmptyFragmentAfterSentence(_clearScreenAfterSentenceEnd);

	QTime timer;
	timer.start();
	load(parser.parse(CFileDecoder::readDataAndDecodeText(filePath)));
	qDebug() << "Parsing" << filePath << "took" << QString::number(timer.elapsed() / 1000.0f, 'f', 1) << "seconds";

	if (!_textFragments.empty())
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
	const size_t numWords = _textFragments.size();
	return numWords ? _position / (long double)numWords : 0.0;
}

size_t CReader::timeRemainingSeconds() const
{
	//const float basePausePerWordSeconds = 60.0f / _speedWpm;
	//const float theoreticalSeconds = (_textFragments.size() - _position) * basePausePerWordSeconds;
	const float actualSeconds = std::accumulate(_textFragments.cbegin() + _position, _textFragments.cend(), 0.0f, [](float acc, const TextFragmentWithPause& fragment) {
		return acc + fragment._pauseAfter / 1000.0f;
	});

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
	return _textFragments.size();
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
	if (_position >= _textFragments.size())
		return;

	_position = wordIndex;
	_interface->updateDisplay(_position);
	_interface->updateInfo();
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
	if (_position >= _textFragments.size())
	{
		resetAndStop();
	}
	else if (_state == Reading)
	{
		_interface->updateDisplay(_position);
		_interface->updateInfo();
		// Queue up the next word
		_readingTimer.start(_textFragments[_position]._pauseAfter);
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
	for (auto& fragment: _textFragments)
		fragment._pauseAfter = pauseForFragment(fragment._textFragment);

	_interface->updateInfo();
}