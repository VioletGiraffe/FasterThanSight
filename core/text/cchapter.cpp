#include "cchapter.h"

#include <numeric>

static const size_t fragmentsPerParagraph = 100;

Paragraph&Chapter::addEmptyParagraph()
{
	_paragraphs.emplace_back();
	_paragraphs.back()._fragments.reserve(fragmentsPerParagraph);
	return _paragraphs.back();
}

size_t Chapter::firstFragmentNumber() const
{
	return _paragraphs.front().firstFragmentNumber();
}

size_t Chapter::lastFragmentNumber() const
{
	return _paragraphs.back().lastFragmentNumber();
}

size_t Chapter::paragraphsCount() const
{
	return _paragraphs.size();
}

size_t Chapter::wordCount() const
{
	const size_t count = lastFragmentNumber() - firstFragmentNumber() + 1;
	assert(count == std::accumulate(_paragraphs.cbegin(), _paragraphs.cend(), (size_t)0, [](size_t acc, const Paragraph& p){
			   return acc + p._fragments.size();
		   }));

	return count;
}

void Chapter::reconstructText(QString& text, std::vector<int>& firstCharacterIndexForFragment) const
{
	for (const auto& paragraph: _paragraphs)
	{
		paragraph.reconstructText(text, firstCharacterIndexForFragment);
		text += '\n';
	}

	if (!text.isEmpty())
		text += '\n';
}
