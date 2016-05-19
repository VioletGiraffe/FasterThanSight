#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class CTextParser
{
public:
	std::vector<TextFragment> parse(const QString& text);

private:
	void finalizeFragment();

private:
	std::vector<TextFragment> _fragments;
	QString _buffer;
	bool _wordEnded = false;
	bool _quoteOpened = false;
	TextFragment::Delimiter _lastDelimiter = TextFragment::Space;
};
