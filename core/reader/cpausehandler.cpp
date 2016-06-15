#include "cpausehandler.h"

#include "assert/advanced_assert.h"
#include "settings.h"

const CPauseHandler &CPauseHandler::instance()
{
	static CPauseHandler handler;
	return handler;
}

float CPauseHandler::pauseFactorForDelimiter(const TextFragment::Delimiter delimiter) const
{
	const auto it = _pauseForDelimiter.find(delimiter);
	assert_and_return_r(it != _pauseForDelimiter.end(), _pauseForDelimiter.at(TextFragment::Space));

	return it->second;
}

void CPauseHandler::loadFromSettings()
{
	_pauseForDelimiter = std::map<TextFragment::Delimiter, float> {
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
		{TextFragment::Newline, 0.88f} // All too often TXT files have new lines for formatting rather than for semantics, so it's best to process them same as space
	};
}

void CPauseHandler::storeToSettings()
{

}

CPauseHandler::CPauseHandler()
{
	loadFromSettings();
}
