#include "ctextfragment.h"

#include <cmath>
#include <algorithm>

TextFragment::TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter /*= NoDelimiter*/) : _word(word), _punctuationText(punctuation), _delimitier(delimiter)
{
}

inline int pivot(TextFragment::PivotCalculationMethod method, int wordLength)
{
	switch (method)
	{
	case TextFragment::pcmMagic:
		switch (wordLength)
		{
		case 1:
			return 0; // First letter
		case 2:
		case 3:
		case 4:
		case 5:
			return 1; // Second letter
		case 6:
		case 7:
		case 8:
		case 9:
			return 2; // Third letter
		case 10:
		case 11:
		case 12:
		case 13:
			return 3; // Fourth letter
		case 0:
			return -1;
		default:
			return 4; // Fifth letter
		}
	case TextFragment::pcmMiddle:
		return std::max(wordLength / 2 - 1, 0);
	case TextFragment::pcmQuarter:
		return std::max((int)std::round(wordLength / 4.0f) - 1, 0);
	case TextFragment::pcmSquareRoot:
		return std::max((int)round(std::sqrt(wordLength)) - 1, 0);
	case TextFragment::pcmCubicRoot:
		return std::max((int)round(std::pow(wordLength, 0.33333f)) - 1, 0);
	case TextFragment::pcmLogarithm:
		return std::max((int)round(std::log2(wordLength)) - 1, 0);
	default:
		return std::max(wordLength / 2 - 1, 0);
	}
}

int TextFragment::pivotLetterIndex(PivotCalculationMethod method) const
{
	const int wordLength = _word.length();
	int pivotIndex = pivot(method, wordLength);

	if (pivotIndex == -1)
		return -1;
	else if (wordLength > 1 && !_word[pivotIndex].isLetterOrNumber())
	{
		if (pivotIndex > wordLength/2)
			return pivotIndex - 1;
		else if (pivotIndex < wordLength/2)
			return pivotIndex + 1;
		else
			return pivotIndex - 1;
	}
	else
		return pivotIndex;
}

QString TextFragment::word() const
{
	return _word;
}

QString TextFragment::punctuation() const
{
	return _punctuationText;
}

TextFragment::Delimiter TextFragment::delimiter() const
{
	return _delimitier;
}
