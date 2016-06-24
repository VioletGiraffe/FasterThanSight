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

	_fragmentCounter = 0;
	_parsedText.clear();

	Paragraph paragraph;
	auto& fragments = paragraph._fragments;

	Chapter chapter;
	chapter._paragraphs.reserve(fixedText.length() / 200); // Empirical value for Symbols/Paragraph ratio

	for (QChar ch: fixedText)
	{
		if (ch == '\r')
			continue;

		const auto delimiterItem = delimiters.find({ch, TextFragment::NoDelimiter});

		if (delimiterItem == delimiters.end()) // Not a delimiter
		{
			if (_wordEnded) // This is the first letter of a new word
				finalizeFragment(fragments);

			_lastDelimiter = TextFragment::NoDelimiter;
			_wordBuffer += ch;
		}
		else // This is a delimiter. Append it to the current word.
		{
			// The opening quote is not a delimiter; the closing one is.
			if (delimiterItem->delimiterType == TextFragment::Newline)
			{
				chapter._paragraphs.push_back(paragraph);
				fragments.clear();
				fragments.reserve(fixedText.length() / 8); // Average English word is 5; 8 is tested to be just as fast and probably won't cause any overhead
				if (_lastDelimiter == TextFragment::NoDelimiter && _wordBuffer == _wordBuffer.toUpper()) // A whole line in uppercase is likely the chapter name
				{
					// Finalize the previous chapter
					_parsedText.addChapter(chapter);

					// Start the new one
					chapter = Chapter();
					chapter.name = _wordBuffer;
				}
			}

			if (delimiterItem->delimiterType == TextFragment::Quote)
			{
				_quoteOpened = !_quoteOpened;
				if (_quoteOpened) // This is an opening quote! Dump the previously accumulated fragment and assign this quote to the new one.
				{
					finalizeFragment(fragments);
					_wordBuffer += ch;
				}
				else // Business as usual
				{
					// Don't let space, newline and quote in e. g. ", " override other punctuation marks
					if (priority(delimiterItem->delimiterType) >= priority(_lastDelimiter))
						_lastDelimiter = delimiterItem->delimiterType;

					_wordEnded = true;
					_delimitersBuffer += ch;
				}
			}
			else
			{
				// Don't let space, newline and quote in e. g. ", " override other punctuation marks
				if (priority(delimiterItem->delimiterType) >= priority(_lastDelimiter))
					_lastDelimiter = delimiterItem->delimiterType;

				_wordEnded = true;
				_delimitersBuffer += ch;
			}
		}
	}

	finalizeFragment(fragments);
	chapter._paragraphs.push_back(paragraph);
	_parsedText.addChapter(chapter);

	return _parsedText;
}

void CTextParser::setAddEmptyFragmentAfterSentence(bool add)
{
	_addEmptyFragmentAfterSentenceEnd = add;
}

void CTextParser::finalizeFragment(std::vector<const IndexedFragment>& fragmentsContainer)
{
	Paragraph paragraph;
	_delimitersBuffer = _delimitersBuffer.trimmed();
	if (!_delimitersBuffer.isEmpty() || !_wordBuffer.isEmpty())
	{
		TextFragment fragment(_wordBuffer, _delimitersBuffer, _lastDelimiter);
		if (_addEmptyFragmentAfterSentenceEnd && fragment.isEndOfSentence())
		{
			fragmentsContainer.push_back({{_wordBuffer, _delimitersBuffer, TextFragment::Comma}, _fragmentCounter});
			
			++_fragmentCounter;
			// Moving the end-of-sentence delimiter off to a dummy fragment with no text - just so that we can fade the text out and hold the screen empty for a bit

			fragmentsContainer.push_back({{QString::null, QString::null, _lastDelimiter}, _fragmentCounter});
			++_fragmentCounter;
		}
		else
		{
			fragmentsContainer.push_back({fragment, _fragmentCounter});
			++_fragmentCounter;
		}
	}

	_wordEnded = false;
	_delimitersBuffer.clear();
	_wordBuffer.clear();
}
