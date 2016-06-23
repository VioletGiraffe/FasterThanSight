#pragma once

#include "../text/ctextfragment.h"

#include <vector>

class QIODevice;

class CTxtFileDecoder
{
public:
	static QString readDataAndDecodeText(QIODevice& device);
};
