#include "creader.h"
#include "../parser/cfiledecoder.h"
#include "../parser/ctextparser.h"
#include "settings/csettings.h"
#include "settings.h"
#include "assert/advanced_assert.h"

#include <math.h>

#include <algorithm>
#include <map>
#include <iterator>

CReader::CReader(ReaderInterface* interface) : _interface(interface)
{
	assert_r(_interface);

	_readingTimer.setSingleShot(true);
	QObject::connect(&_readingTimer, &QTimer::timeout, [this](){
		readNextFragment();
	});

	_speedWpm = CSettings().value(READER_READING_SPEED_SETTING, READER_READING_SPEED_DEFAULT).toUInt();
}

void CReader::load(const std::vector<TextFragment>& textFragments)
{
	resetAndStop();

	_textFragments.clear();
	std::transform(textFragments.cbegin(), textFragments.cend(), std::back_inserter(_textFragments), [this](const TextFragment& fragment) -> TextFragmentWithPause {
		return TextFragmentWithPause{fragment, 0};
	});

	updatePauseValues();
}

void CReader::loadFromFile(const QString& filePath)
{
	CTextParser parser;
	load(parser.parse(CFileDecoder::readDataAndDecodeText(filePath)));
}

CReader::State CReader::state() const
{
	return _state;
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

void CReader::resetAndStop()
{
	pauseReading();
	_position = 0;
}

void CReader::goToWord(size_t wordIndex)
{
	_position = wordIndex;
	_interface->updateDisplay(_position);
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
		// Queue up the next word
		_readingTimer.start(_textFragments[_position]._pauseAfter);
		++_position;
	}
}

// Calculates the pause after the specific fragment in ms
size_t CReader::pauseForFragment(const TextFragment& fragment) const
{
	static const std::map<TextFragment::Delimiter, float /*pauseCoefficient*/> pauseForDelimiter {
		{TextFragment::NoDelimiter, 0.0f},
		{TextFragment::Space, 0.88f},
		{TextFragment::Comma, 1.5f},
		{TextFragment::Point, 2.1f},
		{TextFragment::ExclamationMark, 2.1f},
		{TextFragment::QuestionMark, 2.1f},
		{TextFragment::Dash, 1.5f},
		{TextFragment::Colon, 1.8f},
		{TextFragment::Semicolon, 2.1f},
		{TextFragment::Ellipsis, 2.8f},
		{TextFragment::Bracket, 1.5f},
		{TextFragment::Quote, 1.5f},
		{TextFragment::Newline, 0.88f} // All too often TXT files have new lines for line with formatting rather than for actual semantical formatting, so it's best to read them same as space
	};

	const auto it = pauseForDelimiter.find(fragment.delimiter());
	assert(it != pauseForDelimiter.end());

	const size_t basePause = 60 * 1000 / _speedWpm;
	const int wordLength = fragment.word().length();
	return static_cast<size_t>(it->second * basePause * (wordLength > 6 ? (wordLength - 6) * 1.2f : 1.0f));
}

// Recalculate all the pauses for the entire text
void CReader::updatePauseValues()
{
	for (auto& fragment: _textFragments)
		fragment._pauseAfter = pauseForFragment(fragment._textFragment);
}
