#include "ctxtfiledecoder.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include <QIODevice>
#include <QStringConverter>
RESTORE_COMPILER_WARNINGS

QString CTxtFileDecoder::readDataAndDecodeText(QIODevice& device)
{
	assert_and_return_r(device.isOpen(), QString());

	const QByteArray textData = device.readAll();

	QStringDecoder decoder = QStringDecoder(QStringConverter::Utf8);
	const QString text = decoder.decode(textData);
	if (!decoder.hasError())
		return text;

	decoder = QStringDecoder(QStringConverter::System);
	return decoder.decode(textData);
}
