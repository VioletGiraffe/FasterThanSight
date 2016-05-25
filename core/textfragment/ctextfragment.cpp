#include "ctextfragment.h"

TextFragment::TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter /*= NoDelimiter*/) : _word(word), _punctuationText(punctuation), _delimitier(delimiter)
{
}

int TextFragment::pivotLetterIndex() const
{
	int pivotIndex = -1;
	const int wordLength = _word.length();
	switch (wordLength)
	{
	case 1:
		pivotIndex = 0; // First letter
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		pivotIndex = 1; // Second letter
		break;
	case 6:
	case 7:
	case 8:
	case 9:
		pivotIndex = 2; // Third letter
		break;
	case 10:
	case 11:
	case 12:
	case 13:
		pivotIndex = 3; // Fourth letter
		break;
	case 0:
		pivotIndex = -1;
		break;
	default:
		pivotIndex = 4; // Fifth letter
		break;
	}

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
