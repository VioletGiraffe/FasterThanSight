#include "cstructuredtext.h"

#include <algorithm>

CStructuredText::CStructuredText()
{

}

void CStructuredText::addChapter(const QString& name, const std::vector<const Paragraph>& paragraphs)
{
	_chapters.push_back({name, paragraphs});
}

const TextFragment& CStructuredText::fragment(size_t fragmentIndex) const
{
	static const TextFragment dummy;

	auto chapterIterator = std::lower_bound(_chapters.begin(), _chapters.end(), fragmentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});

	return dummy;
}
