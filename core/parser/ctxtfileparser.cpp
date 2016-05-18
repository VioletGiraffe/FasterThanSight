#include "ctxtfileparser.h"
#include "assert/advanced_assert.h"

#include <QFile>
#include <QTextCodec>

#include <set>

std::vector<TextFragment> CTxtFileParser::parse(QIODevice& device)
{
	const QString text = readText(device);

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
		{'—', TextFragment::Dash},
		// TODO:
		// {"...", TextFragment::Ellipsis},
		{'⋯', TextFragment::Ellipsis},
		{'…', TextFragment::Ellipsis},
		{'!', TextFragment::ExclamationMark},
		{'\n', TextFragment::Newline},
		{'?', TextFragment::QuestionMark},

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
				fragments.emplace_back(buffer, lastDelimiter);
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
			buffer += ch;
		}
	}

	return fragments;
}

QString CTxtFileParser::readText(QIODevice& device)
{
	assert_and_return_r(device.isOpen(), QString());

	QTextCodec* codec = QTextCodec::codecForName("UTF-8");
	if (!codec)
		return QString();

	QTextCodec::ConverterState state;
	const QByteArray textData = device.readAll();
	QString text(codec->toUnicode(textData.constData(), textData.size(), &state));
	if (state.invalidChars == 0)
		return text; // Valid UTF-8 text

	codec = QTextCodec::codecForLocale();
	if (!codec)
		return QString();

	return codec->toUnicode(textData);
}
