#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class QIODevice;

class CTxtFileParser
{
public:
	static std::vector<TextFragment> parse(QIODevice& device);

private:
	static QString readText(QIODevice& device);
};
