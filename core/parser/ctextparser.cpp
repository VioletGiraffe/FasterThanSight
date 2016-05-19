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

	std::vector<TextFragment> fragments;

	QString buffer;
	bool wordEnded = false;
	bool quoteOpened = false; // The opening quote is not a delimiter; the closing one is.
	TextFragment::Delimiter lastDelimiter = TextFragment::Space;
	for (QChar ch: text)
	{
		if (ch == '\r')
			continue;

		const auto it = delimiters.find({ch, TextFragment::Space});
		if (it == delimiters.end()) // Not a delimiter
		{
			if (wordEnded) // This is the first letter of a new word
			{
				fragments.emplace_back(buffer.trimmed(), lastDelimiter);
				wordEnded = false;
				buffer = ch;
			}
			else
				buffer += ch;
		}
		else // This is a delimiter. Append it to the current word.
		{
			lastDelimiter = it->delimiterType;
			wordEnded = true;

			// The opening quote is not a delimiter; the closing one is.
			if (lastDelimiter == TextFragment::Quote)
			{
				quoteOpened = !quoteOpened;
				if (quoteOpened)
					wordEnded = false;
			}

			buffer += ch;
		}
	}

	return fragments;
}
