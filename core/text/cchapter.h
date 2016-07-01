#pragma once

#include "cparagraph.h"

struct Chapter {
	Paragraph& addEmptyParagraph();
	size_t firstFragmentNumber() const;
	size_t lastFragmentNumber() const;
	size_t paragraphsCount() const;
	size_t wordCount() const;

	QString reconstructText(bool richText) const;

	QString name;
	std::vector<Paragraph> _paragraphs;
};
