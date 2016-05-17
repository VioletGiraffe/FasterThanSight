#include "ctxtfileparser.h"
#include "assert/advanced_assert.h"

#include <QFile>
#include <QTextCodec>

CTxtFileParser::CTxtFileParser()
{

}

std::vector<TextFragment> CTxtFileParser::parse(QIODevice& device)
{
	const QString text = readText(device);

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
