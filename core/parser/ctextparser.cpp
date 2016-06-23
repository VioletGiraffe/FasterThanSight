#include "ctextparser.h"

#include <set>

inline int priority(TextFragment::Delimiter delimiter)
{
	return delimiter;
}

CStructuredText CTextParser::parse(const QString& text)
{
	struct Delimiter {
		QChar delimiterCharacter;
		TextFragment::Delimiter delimiterType;

		inline bool operator< (const Delimiter& other) const {
			return delimiterCharacter < other.delimiterCharacter;
		}
	};

	QString fixedText = text;
	fixedText
		.replace(QChar(0x00A0), ' ') // Non-breaking space -> regular space
		.replace(". . .", QChar(0x2026)) // 3 space-separated dots -> ellipsis // TODO: regexp
		.replace("...", QChar(0x2026)); // 3 dots -> ellipsis // TODO: regexp

	static const std::set<Delimiter> delimiters {
		{' ', TextFragment::Space},
		{'.', TextFragment::Point},
		{':', TextFragment::Colon},
		{';', TextFragment::Semicolon},
		{',', TextFragment::Comma},
		// TODO: dash should be ignored unless it has an adjacent space!
		{'-', TextFragment::Dash},
		{0x2014, TextFragment::Dash},
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

	_parsedText.clear();

	for (QChar ch: fixedText)
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
					_wordBuffer += ch;
				}
				else // Business as usual
				{
					// Don't let space, newline and quote in e. g. ", " override other punctuation marks
					if (priority(it->delimiterType) >= priority(_lastDelimiter))
						_lastDelimiter = it->delimiterType;

					_wordEnded = true;
					_delimitersBuffer += ch;
				}
			}
			else
			{
				// Don't let space, newline and quote in e. g. ", " override other punctuation marks
				if (priority(it->delimiterType) >= priority(_lastDelimiter))
					_lastDelimiter = it->delimiterType;

				_wordEnded = true;
				_delimitersBuffer += ch;
			}
		}
	}

	finalizeFragment();

	return _parsedText;
}

void CTextParser::setAddEmptyFragmentAfterSentence(bool add)
{
	_addEmptyFragmentAfterSentenceEnd = add;
}

void CTextParser::finalizeFragment()
{
	Paragraph paragraph;
	_delimitersBuffer = _delimitersBuffer.trimmed();
	if (!_delimitersBuffer.isEmpty() || !_wordBuffer.isEmpty())
	{
		TextFragment fragment(_wordBuffer, _delimitersBuffer, _lastDelimiter);
		if (_addEmptyFragmentAfterSentenceEnd && fragment.isEndOfSentence())
		{
			paragraph._fragments.push_back({{_wordBuffer, _delimitersBuffer, TextFragment::Comma}, _fragmentCounter});
			
			++_fragmentCounter;
			// Moving the end-of-sentence delimiter off to a dummy fragment with no text - just so that we can fade the text out and hold the screen empty for a bit

			paragraph._fragments.push_back({{QString::null, QString::null, _lastDelimiter}, _fragmentCounter});
			++_fragmentCounter;
		}
		else
		{
			paragraph._fragments.push_back({fragment, _fragmentCounter});
			++_fragmentCounter;
		}

		_parsedText.addChapter(QString::number(_fragmentCounter), {paragraph});
	}

	_wordEnded = false;
	_delimitersBuffer.clear();
	_wordBuffer.clear();
}
