#include "ctextfragment.h"

TextFragment::TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter /*= NoDelimiter*/) : _word(word), _punctuationText(punctuation), _delimitier(delimiter)
{
}

int TextFragment::pivotLetterIndex() const
{
	return 0;
// 	switch (_text.length()) {
// 	case 1:
// 		bestLetter = 1; // first
// 		break;
// 	case 2:
// 	case 3:
// 	case 4:
// 	case 5:
// 		bestLetter = 2; // second
// 		break;
// 	case 6:
// 	case 7:
// 	case 8:
// 	case 9:
// 		bestLetter = 3; // third
// 		break;
// 	case 10:
// 	case 11:
// 	case 12:
// 	case 13:
// 		bestLetter = 4; // fourth
// 		break;
// 	default:
// 		bestLetter = 5; // fifth
}

QString TextFragment::word() const
{
	return _word;
}

QString TextFragment::punctuation() const
{
	return _punctuationText;
}

QString TextFragment::entireTextFragment() const
{
	return _word + _punctuationText;
}

TextFragment::Delimiter TextFragment::delimiter() const
{
	return _delimitier;
}
