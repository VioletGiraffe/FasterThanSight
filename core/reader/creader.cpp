#include "creader.h"
#include "../parser/cfiledecoder.h"
#include "../parser/ctextparser.h"

CReader::CReader(ReaderInterface* interface) : _interface(interface)
{
	QObject::connect(&_readingTimer, &QTimer::timeout, [this](){
		readNextFragment();
	});
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
	load(CTextParser::parse(CFileDecoder::readDataAndDecodeText(filePath)));
}

void CReader::resumeReading()
{
	_readingTimer.start(60 * 1000 / _speedWpm);
	_interface->stateChanged(Reading);
}

void CReader::pauseReading()
{
	_readingTimer.stop();
	_interface->stateChanged(Paused);
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
	_speedWpm = wpm;
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
