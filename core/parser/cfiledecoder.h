#pragma once

#include <QString>

class CFileDecoder
{
public:
	static QString readDataAndDecodeText(const QString& filePath);
};
