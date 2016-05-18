#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class CFileParser
{
public:
	static std::vector<TextFragment> parse(const QString& filePath);
};
