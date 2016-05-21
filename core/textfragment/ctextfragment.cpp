#include "ctextfragment.h"

TextFragment::TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter /*= NoDelimiter*/) : _word(word), _punctuationText(punctuation), _delimitier(delimiter)
{
}

int TextFragment::pivotLetterIndex() const
{
	switch (_word.length())
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
		break;
	case 0:
		return -1;
	default:
		return 5; // Fifth letter
	}
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
