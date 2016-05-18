#include "cfileparser.h"

#include "ctxtfileparser.h"

#include <QDebug>
#include <QFileInfo>

std::vector<TextFragment> CFileParser::parse(const QString& filePath)
{
	QFileInfo fileInfo(filePath);
	if (!fileInfo.exists())
	{
		qDebug() << __FUNCTION__ << filePath << "doesn't exist";
		return std::vector<TextFragment>();
	}
	else if (!fileInfo.isFile())
	{
		qDebug() << __FUNCTION__ << filePath << "is not a file";
		return std::vector<TextFragment>();
	}

	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
	{
		qDebug() << __FUNCTION__ << "failed to open the file" << filePath;
		return std::vector<TextFragment>();
	}

	const QString ext = fileInfo.suffix().toLower();
	if (ext == "txt")
	{
		return CTxtFileParser::parse(file);
	}
	else
		qDebug() << __FUNCTION__ << "unsupported extension" << ext;

	return std::vector<TextFragment>();
}
