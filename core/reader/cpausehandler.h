#pragma once

#include "../text/ctextfragment.h"

#include <map>

class CPauseHandler
{
public:
	static const CPauseHandler& instance();

	float pauseFactorForDelimiter(const TextFragment::Delimiter delimiter) const;

	void loadFromSettings();
	void storeToSettings();

private:
	CPauseHandler();

private:
	std::map<TextFragment::Delimiter, float /*pauseFactor*/> _pauseForDelimiter;
};
