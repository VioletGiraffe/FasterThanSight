#include "ctxtfiledecoder.h"
#include "assert/advanced_assert.h"

#include <QIODevice>
#include <QTextCodec>

QString CTxtFileDecoder::readDataAndDecodeText(QIODevice& device)
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
