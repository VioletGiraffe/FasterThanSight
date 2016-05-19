#pragma once

#include "../textfragment/ctextfragment.h"

#include <vector>

class CTextParser
{
public:
	static std::vector<TextFragment> parse(const QString& text);
};
