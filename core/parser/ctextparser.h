#pragma once

#include "../text/cstructuredtext.h"

#include <vector>

class CTextParser
{
public:
	CStructuredText parse(const QString& text);

	void setAddEmptyFragmentAfterSentence(bool add);

private:
	void finalizeFragment(std::vector<const IndexedFragment>& fragmentsContainer);

private:
	 CStructuredText _parsedText;
	 size_t _fragmentCounter = 0;

	QString _wordBuffer;
	QString _delimitersBuffer;
	bool _wordEnded = false;
	bool _quoteOpened = false;
	TextFragment::Delimiter _lastDelimiter = TextFragment::NoDelimiter;
	bool _addEmptyFragmentAfterSentenceEnd = false;
};
