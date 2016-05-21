#include "creader.h"
#include "../parser/cfiledecoder.h"
#include "../parser/ctextparser.h"
#include "settings/csettings.h"
#include "settings.h"
#include "assert/advanced_assert.h"

#include <algorithm>
#include <map>
#include <iterator>

CReader::CReader(ReaderInterface* interface) : _interface(interface)
{
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
	std::transform(textFragments.cbegin(), textFragments.cend(), std::back_inserter(_textFragments), [this](const TextFragment& fragment){
		return fragment;
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

size_t CReader::readingSpeed() const
{
	return _speedWpm;
}

void CReader::setReadingSpeed(size_t wpm)
{
	CSettings().setValue(READER_READING_SPEED_SETTING, wpm);
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
		{TextFragment::Space, 0.9f},
		{TextFragment::Comma, 1.6f},
		{TextFragment::Point, 2.2f},
		{TextFragment::ExclamationMark, 2.2f},
		{TextFragment::QuestionMark, 2.2f},
		{TextFragment::Dash, 1.6f},
		{TextFragment::Colon, 1.9f},
		{TextFragment::Semicolon, 2.2f},
		{TextFragment::Ellipsis, 3.0f},
		{TextFragment::Bracket, 1.6f},
		{TextFragment::Quote, 1.6f},
		{TextFragment::Newline, 0.9f} // All too often TXT files have new lines for line with formatting rather than for actual semantical formatting, so it's best to read them same as space
	};

	const auto it = pauseForDelimiter.find(fragment._delimitier);
	assert(it != pauseForDelimiter.end());

	const size_t basePause = 60 * 1000 / _speedWpm;
	return static_cast<size_t>(it->second * basePause);
}

// Recalculate all the pauses for the entire text
void CReader::updatePauseValues()
{
	for (auto& fragment: _textFragments)
		fragment._pauseAfter = pauseForFragment(fragment._textFragment);
}
