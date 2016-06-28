#pragma once

#include "../text/ctextfragment.h"

#include <map>

class CPauseHandler
{
public:
	static CPauseHandler& instance();

	float pauseFactorForDelimiter(const TextFragment::Delimiter delimiter) const;
	void updatePauseFactor(const TextFragment::Delimiter delimiter, const float factor);
	const std::map<TextFragment::Delimiter, float /*pauseFactor*/> pauseScalingValues() const;

	void loadFromSettings();
	void storeToSettings() const;

private:
	CPauseHandler();

private:
	std::map<TextFragment::Delimiter, float /*pauseFactor*/> _pauseForDelimiter;
};
