#include "ctextparser.h"

#include <set>

std::vector<TextFragment> CTextParser::parse(const QString& text)
{
	struct Delimiter {
		QChar delimiterCharacter;
		TextFragment::Delimiter delimiterType;

		inline bool operator< (const Delimiter& other) const {
			return delimiterCharacter < other.delimiterCharacter;
		}
	};

	static const std::set<Delimiter> delimiters {
		{' ', TextFragment::Space},
		{'.', TextFragment::Point},
		{':', TextFragment::Colon},
		{';', TextFragment::Semicolon},
		{',', TextFragment::Comma},
		// TODO: dash should be ignored unless it has an adjacent space!
		{'-', TextFragment::Dash},
		{0x2014, TextFragment::Dash},
		// TODO:
		// {"...", TextFragment::Ellipsis},
		{0x2026, TextFragment::Ellipsis},
		{'!', TextFragment::ExclamationMark},
		{'\n', TextFragment::Newline},
		{'?', TextFragment::QuestionMark},
		{'\"', TextFragment::Quote},

		{')', TextFragment::Bracket},
		{'(', TextFragment::Bracket},
		{'[', TextFragment::Bracket},
		{']', TextFragment::Bracket},
		{'{', TextFragment::Bracket},
		{'}', TextFragment::Bracket}
	};

	_fragments.clear();

	for (QChar ch: text)
	{
		if (ch == '\r')
			continue;

		const auto it = delimiters.find({ch, TextFragment::NoDelimiter});

		if (it == delimiters.end()) // Not a delimiter
		{
			if (_wordEnded) // This is the first letter of a new word
				finalizeFragment();

			_lastDelimiter = TextFragment::NoDelimiter;
			_wordBuffer += ch;
		}
		else // This is a delimiter. Append it to the current word.
		{
			// The opening quote is not a delimiter; the closing one is.
			if (it->delimiterType == TextFragment::Quote)
			{
				_quoteOpened = !_quoteOpened;
				if (_quoteOpened) // This is an opening quote! Dump the previously accumulated fragment and assign this quote to the new one.
				{
					finalizeFragment();
				}
				else // Business as usual
				{
					// Don't let space and newline in e. g. ", " override other punctuation marks
					if ((it->delimiterType != TextFragment::Space && it->delimiterType != TextFragment::Newline) || _lastDelimiter == TextFragment::NoDelimiter)
						_lastDelimiter = it->delimiterType;

					_wordEnded = true;
				}
			}
			else
			{
				// Don't let space and newline in e. g. ", " override other punctuation marks
				if ((it->delimiterType != TextFragment::Space && it->delimiterType != TextFragment::Newline) || _lastDelimiter == TextFragment::NoDelimiter)
					_lastDelimiter = it->delimiterType;

				_wordEnded = true;
			}

			_delimitersBuffer += ch;
		}
	}

	finalizeFragment();

	return _fragments;
}

void CTextParser::finalizeFragment()
{
	_delimitersBuffer = _delimitersBuffer.trimmed();
	if (!_delimitersBuffer.isEmpty() || !_wordBuffer.isEmpty())
		_fragments.emplace_back(_wordBuffer, _delimitersBuffer, _lastDelimiter);

	_wordEnded = false;
	_delimitersBuffer.clear();
	_wordBuffer.clear();
}
