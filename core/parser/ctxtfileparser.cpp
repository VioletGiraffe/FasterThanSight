#include "ctxtfileparser.h"
#include "assert/advanced_assert.h"

#include <QFile>
#include <QTextCodec>

#include <set>

CTxtFileParser::CTxtFileParser()
{

}

std::vector<TextFragment> CTxtFileParser::parse(QIODevice& device)
{
	const QString text = readText(device);

	struct Delimiter {
		QString delimiterCharacter;
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
		{'-', TextFragment::Dash},
		{"...", TextFragment::Ellipsis},
		{"⋯", TextFragment::Ellipsis},
		{"…", TextFragment::Ellipsis},
		{"!", TextFragment::ExclamationMark},
		{"\n", TextFragment::Newline},
		{"?", TextFragment::QuestionMark},

		{')', TextFragment::Bracket},
		{'(', TextFragment::Bracket},
		{'[', TextFragment::Bracket},
		{']', TextFragment::Bracket},
		{'{', TextFragment::Bracket},
		{'}', TextFragment::Bracket}
	};

	std::vector<TextFragment> fragments;

	return fragments;
}

std::vector<TextFragment> CTxtFileParser::parseFile(const QString& filePath)
{
	QFile file(filePath);
	return parse(file);
}

QString CTxtFileParser::readText(QIODevice& device)
{
	if (!device.isOpen())
		assert_and_return_r(device.open(QIODevice::ReadOnly), QString());

	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	if (!codec)
		return QString();

	QTextCodec::ConverterState state;
	const QByteArray textData = device.readAll();
	QString text(codec->toUnicode(textData.constData(), textData.size(), &state));
	if (state.invalidChars == 0)
		return text; // Valid UTF-8 text

	codec = QTextCodec::codecForLocale();
	return codec->toUnicode(textData);
}
