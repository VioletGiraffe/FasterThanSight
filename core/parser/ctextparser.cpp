#include "ctextparser.h"

DISABLE_COMPILER_WARNINGS
#include <QDebug>
RESTORE_COMPILER_WARNINGS

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
	qDebug() << "Parsing text of" << fixedText.length() << "characters";
	_parsedText.setExpectedChaptersCount(fixedText.length() / 20000);

	for (QChar ch: fixedText)
	{
		if (ch == '\r')
			continue;

		const auto delimiterItem = delimiters.find({ch, TextFragment::NoDelimiter});

		if (delimiterItem == delimiters.end()) // Not a delimiter
		{
			if (_wordEnded) // This is the first letter of a new word
				finalizeFragment();

			_lastDelimiter = TextFragment::NoDelimiter;
			_wordBuffer += ch;
		}
		else // This is a delimiter. Append it to the current word.
		{
			// The opening quote is not a delimiter; the closing one is.
			if (delimiterItem->delimiterType == TextFragment::Quote)
			{
				_quoteOpened = !_quoteOpened;
				if (_quoteOpened) // This is an opening quote! Dump the previously accumulated fragment and assign this quote to the new one.
				{
					finalizeFragment();
					_wordBuffer += ch;
				}
				else // Business as usual
				{
					// Don't let space, comma and quote in e. g. ", " override other punctuation marks
					if (priority(delimiterItem->delimiterType) >= priority(_lastDelimiter))
						_lastDelimiter = delimiterItem->delimiterType;

					_wordEnded = true;
					_delimitersBuffer += ch;
				}
			}
			else
			{
				// Don't let space, comma and quote in e. g. ", " override other punctuation marks
				if (priority(delimiterItem->delimiterType) >= priority(_lastDelimiter))
					_lastDelimiter = delimiterItem->delimiterType;

				_wordEnded = true;
				_delimitersBuffer += ch;
			}
		}
	}

	finalizeFragment();

	_parsedText.removeEmptyItems();
	return _parsedText;
}

void CTextParser::setAddEmptyFragmentAfterSentence(bool add)
{
	_addEmptyFragmentAfterSentenceEnd = add;
}

inline bool isUpperCaseText(const QString& str)
{
	bool letterDetected = false;
	for (QChar ch : str)
	{
		if (ch.isLetterOrNumber())
		{
			letterDetected = true;
			if (!ch.isNumber() && !ch.isUpper())
				return false;
		}
	}

	return letterDetected;
}

void CTextParser::finalizeFragment()
{
	_delimitersBuffer = _delimitersBuffer.trimmed();
	if (!_delimitersBuffer.isEmpty() || !_wordBuffer.isEmpty())
	{
		if (_lastDelimiter == TextFragment::Newline)
		{
			// A whole line in uppercase is likely the chapter name.
			// The _delimitersBuffer is already trimmed so no need to worry about it consisting of whitespace without any meaningful characters.
			// This condition has to be checked first so that the chapter name properly starts the new chapter.
			if (_delimitersBuffer.isEmpty() && isUpperCaseText(_wordBuffer))
			{
				// Start the new chapter
				_parsedText.addEmptyChapter(_wordBuffer).addEmptyParagraph();
			}
		}

		// Creating empty containers if necessary
		if (_parsedText.empty()) // This should only happen once, when parsing the first line
			_parsedText.addEmptyChapter(_wordBuffer).addEmptyParagraph();

		TextFragment fragment(_wordBuffer, _delimitersBuffer, _lastDelimiter);
		if (_addEmptyFragmentAfterSentenceEnd && fragment.isEndOfSentence())
		{
			_parsedText.lastParagraph().addFragment({_wordBuffer, _delimitersBuffer, TextFragment::Comma}, _fragmentCounter++);

			// Moving the end-of-sentence delimiter off to a dummy fragment with no text - just so that we can fade the text out and hold the screen empty for a bit
			_parsedText.lastParagraph().addFragment({QString::null, QString::null, _lastDelimiter}, _fragmentCounter++);
		}
		else
		{
			_parsedText.lastParagraph().addFragment(fragment, _fragmentCounter++);
		}

		if (_lastDelimiter == TextFragment::Newline)
			// Start a new paragraph
			_parsedText.lastChapter().addEmptyParagraph();
	}

	_wordEnded = false;
	_delimitersBuffer.clear();
	_wordBuffer.clear();
}
