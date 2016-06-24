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

Chapter& CStructuredText::addEmptyChapter(const QString& name, size_t expectedNumParagraphs /*= 100*/)
{
	_chapters.emplace_back();
	_chapters.back().name = name;
	_chapters.back()._paragraphs.reserve(expectedNumParagraphs);
	return _chapters.back();
}

void CStructuredText::clear()
{
	_chapters.clear();
}

bool CStructuredText::empty() const
{
	return _chapters.empty() || _chapters.front()._paragraphs.empty() || _chapters.front()._paragraphs.front()._fragments.empty();
}

Chapter& CStructuredText::lastChapter()
{
	return _chapters.back();
}

Paragraph& CStructuredText::lastParagraph()
{
	return lastChapter()._paragraphs.back();
}

size_t CStructuredText::previousChapterStartIndex(size_t currentIndex) const
{
	auto chapterIterator = std::lower_bound(_chapters.cbegin(), _chapters.cend(), currentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});
	assert_and_return_r(chapterIterator != _chapters.cend(), currentIndex);

	if (chapterIterator != _chapters.cbegin())
		--chapterIterator;

	return chapterIterator->firstFragmentNumber();
}

size_t CStructuredText::previousParagraphStartIndex(size_t currentIndex) const
{
	const auto chapterIterator = std::lower_bound(_chapters.cbegin(), _chapters.cend(), currentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});
	assert_and_return_r(chapterIterator != _chapters.cend(), currentIndex);

	auto paragraphIterator = std::lower_bound(chapterIterator->_paragraphs.cbegin(), chapterIterator->_paragraphs.cend(), currentIndex, [](const Paragraph& paragraph, size_t index){
		return paragraph.lastFragmentNumber() < index;
	});
	assert_and_return_r(paragraphIterator != chapterIterator->_paragraphs.cend(), currentIndex);

	if (paragraphIterator != chapterIterator->_paragraphs.cbegin())
		--paragraphIterator;

	return paragraphIterator->firstFragmentNumber();
}

size_t CStructuredText::nextParagraphStartIndex(size_t currentIndex) const
{
	const auto chapterIterator = std::lower_bound(_chapters.cbegin(), _chapters.cend(), currentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});
	assert_and_return_r(chapterIterator != _chapters.cend(), currentIndex);

	auto paragraphIterator = std::lower_bound(chapterIterator->_paragraphs.cbegin(), chapterIterator->_paragraphs.cend(), currentIndex, [](const Paragraph& paragraph, size_t index){
		return paragraph.lastFragmentNumber() < index;
	});
	assert_and_return_r(paragraphIterator != chapterIterator->_paragraphs.cend(), currentIndex);

	if (paragraphIterator + 1 != chapterIterator->_paragraphs.cend())
		++paragraphIterator;

	return paragraphIterator->firstFragmentNumber();
}

size_t CStructuredText::nextChapterStartIndex(size_t currentIndex) const
{
	auto chapterIterator = std::lower_bound(_chapters.cbegin(), _chapters.cend(), currentIndex, [](const Chapter& chapter, size_t index){
		return chapter.lastFragmentNumber() < index;
	});
	assert_and_return_r(chapterIterator != _chapters.cend(), currentIndex);

	if (chapterIterator + 1 != _chapters.cend())
		++chapterIterator;

	return chapterIterator->firstFragmentNumber();
}

const std::vector<Chapter>& CStructuredText::chapters() const
{
	return _chapters;
}

size_t CStructuredText::chaptersCount() const
{
	return _chapters.size();
}

size_t CStructuredText::totalFragmentsCount() const
{
	return empty() ? 0 : _chapters.back().lastFragmentNumber() + 1;
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
