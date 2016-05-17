#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class QIODevice;

class CTxtFileParser
{
public:
	CTxtFileParser();

	std::vector<TextFragment> parse(QIODevice& device);
	std::vector<TextFragment> parseFile(const QString& filePath);

private:
	QString readText(QIODevice& device);
};
