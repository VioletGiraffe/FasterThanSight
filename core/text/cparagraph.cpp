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

QString Paragraph::reconstructText(bool richText) const
{
	QString text;
	for (const auto& fragment : _fragments)
	{
		QString fragmentText = fragment.fragment.text();
		if (!fragmentText.endsWith(' '))
			fragmentText += ' ';

		text.append(fragmentText);
	}

	return text;
}
