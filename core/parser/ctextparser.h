#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class CTextParser
{
public:
	std::vector<TextFragment> parse(const QString& text);

	void setAddEmptyFragmentAfterSentence(bool add);

private:
	void finalizeFragment();

private:
	std::vector<TextFragment> _fragments;
	QString _wordBuffer;
	QString _delimitersBuffer;
	bool _wordEnded = false;
	bool _quoteOpened = false;
	TextFragment::Delimiter _lastDelimiter = TextFragment::NoDelimiter;
	bool _addEmptyFragmentAfterSentenceEnd = false;
};
