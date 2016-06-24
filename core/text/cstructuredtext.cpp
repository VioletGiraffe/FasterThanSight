#include "cstructuredtext.h"
#include "assert/advanced_assert.h"

#include <algorithm>

CStructuredText::CStructuredText()
{

}

void CStructuredText::addChapter(const QString& name, const std::vector<Paragraph>& paragraphs)
{
	_chapters.push_back({name, paragraphs});
}

void CStructuredText::addChapter(const Chapter& chapter)
{
	_chapters.push_back(chapter);
}

void CStructuredText::clear()
{
	_chapters.clear();
}

const TextFragment& CStructuredText::fragment(size_t fragmentIndex) const
{
	static const TextFragment dummy;

	const auto chapterIterator = std::lower_bound(_chapters.cbegin(), _chapters.cend(), fragmentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});
	assert_and_return_r(chapterIterator != _chapters.cend(), dummy);

	const auto paragraphIterator = std::lower_bound(chapterIterator->_paragraphs.cbegin(), chapterIterator->_paragraphs.cend(), fragmentIndex, [](const Paragraph& paragraph, size_t index){
		return paragraph.lastFragmentNumber() < index;
	});
	assert_and_return_r(paragraphIterator != chapterIterator->_paragraphs.cend(), dummy);

	const auto fragmentIterator = std::lower_bound(paragraphIterator->_fragments.cbegin(), paragraphIterator->_fragments.cend(), fragmentIndex, [](const IndexedFragment& fragment, size_t index) {
		return fragment.fragmentIndex < index;
	});
	assert_and_return_r(fragmentIterator != paragraphIterator->_fragments.cend(), dummy);

	return fragmentIterator->fragment;
}
