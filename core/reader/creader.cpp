#include "creader.h"
#include "../parser/cfiledecoder.h"
#include "../parser/ctextparser.h"
#include "settings/csettings.h"
#include "settings.h"

CReader::CReader(ReaderInterface* interface) : _interface(interface)
{
	QObject::connect(&_readingTimer, &QTimer::timeout, [this](){
		readNextFragment();
	});

	_speedWpm = CSettings().value(READER_READING_SPEED_SETTING, READER_READING_SPEED_DEFAULT).toUInt();
	_readingTimer.setInterval(60 * 1000 / _speedWpm);
}

void CReader::load(const std::vector<TextFragment>& textFragments)
{
	pauseReading();

	_position = 0;
	std::vector<TextFragment> tmp(textFragments);
	_textFragments.swap(tmp);
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
	_readingTimer.start();

	_state = Reading;
	_interface->stateChanged(_state);
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
	_readingTimer.setInterval(60 * 1000 / _speedWpm);
}

void CReader::readNextFragment()
{
	if (_position >= _textFragments.size())
	{
		resetAndStop();
		return;
	}

	_interface->displayText(_textFragments[_position++]);
}
