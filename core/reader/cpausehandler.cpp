#include "cpausehandler.h"

#include "assert/advanced_assert.h"
#include "settings.h"
#include "settings/csettings.h"

#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

CPauseHandler &CPauseHandler::instance()
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

void CPauseHandler::updatePauseFactor(const TextFragment::Delimiter delimiter, const float factor)
{
	const auto it = _pauseForDelimiter.find(delimiter);
	assert_and_return_r(it != _pauseForDelimiter.end(),);

	it->second = factor;
}

const std::map<TextFragment::Delimiter, float /*pauseFactor*/> CPauseHandler::pauseScalingValues() const
{
	return _pauseForDelimiter;
}

void CPauseHandler::loadFromSettings()
{
	// Defaults
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
		{TextFragment::Newline, 2.1f}
	};

	const QStringList values = CSettings().value(PAUSE_SCALING_SETTING).toStringList();
	for (const QString& item : values)
	{
		auto parsed = item.split('=');
		if (parsed.size() != 2)
		{
			assert_unconditional_r((QString("Invalid pause scaling setting item ") + item).toUtf8().data());
			continue;
		}

		const int id = parsed[0].toInt();
		const auto it = std::find_if(_pauseForDelimiter.begin(), _pauseForDelimiter.end(), [id](const decltype(_pauseForDelimiter)::value_type& mapItem) {
			return mapItem.first == id;
		});

		if (it == _pauseForDelimiter.end())
		{
			assert_unconditional_r((QString("Invalid delimiter item value found in settings: ") + item).toUtf8().data());
			continue;
		}

		const float value = parsed[1].toFloat();
		it->second = value;
	}
}

void CPauseHandler::storeToSettings() const
{
	QStringList data;
	for (const auto& item : _pauseForDelimiter)
		data.push_back(QString::number(item.first.value()) % '=' % QString::number(item.second));

	CSettings().setValue(PAUSE_SCALING_SETTING, data);
}

CPauseHandler::CPauseHandler()
{
	loadFromSettings();
}
