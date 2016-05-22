#include "cfiledecoder.h"

#include "ctxtfiledecoder.h"

DISABLE_COMPILER_WARNINGS
#include <QDebug>
#include <QFileInfo>
RESTORE_COMPILER_WARNINGS

QString CFileDecoder::readDataAndDecodeText(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists())
	{
		qDebug() << __FUNCTION__ << filePath << "doesn't exist";
		return QString();
	}
	else if (!fileInfo.isFile())
	{
		qDebug() << __FUNCTION__ << filePath << "is not a file";
		return QString();
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << __FUNCTION__ << "failed to open the file" << filePath;
		return QString();
	}

	const QString ext = fileInfo.suffix().toLower();
	if (ext == "txt")
	{
		return CTxtFileDecoder::readDataAndDecodeText(file);
	}
	else
		qDebug() << __FUNCTION__ << "unsupported extension" << ext;

	return QString();
}
