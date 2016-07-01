#include "cparagraph.h"

void Paragraph::addFragment(const TextFragment& fragment, size_t index)
{
	_fragments.push_back({fragment, index});
}

size_t Paragraph::firstFragmentNumber() const
{
	return _fragments.front().fragmentIndex;
}

size_t Paragraph::lastFragmentNumber() const
{
	return _fragments.back().fragmentIndex;
}

size_t Paragraph::fragmentsCount() const
{
	return _fragments.size();
}

void Paragraph::reconstructText(QString& text, std::vector<int>& firstCharacterIndexForFragment) const
{
	for (const auto& fragment : _fragments)
	{
		QString fragmentText = fragment.fragment.text();
		if (!fragmentText.endsWith(' '))
			fragmentText += ' ';

		firstCharacterIndexForFragment[fragment.fragmentIndex] = text.length();
		text.append(fragmentText);
	}
}
