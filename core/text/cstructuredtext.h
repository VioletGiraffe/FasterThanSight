#pragma once

#include "ctextfragment.h"

#include <vector>

struct IndexedFragment {
	TextFragment fragment;
	size_t fragmentIndex;
};

struct Paragraph {
	inline void addFragment(const TextFragment& fragment, size_t index)
	{
		_fragments.push_back({fragment, index});
	}

	inline size_t firstFragmentNumber() const
	{
		return _fragments.front().fragmentIndex;
	}

	inline size_t lastFragmentNumber() const
	{
		return _fragments.back().fragmentIndex;
	}

	inline size_t fragmentsCount() const
	{
		return _fragments.size();
	}

	std::vector<IndexedFragment> _fragments;
};

struct Chapter {
	inline Paragraph& addEmptyParagraph(size_t numWordsExpected = 0)
	{
		_paragraphs.emplace_back();
		_paragraphs.back()._fragments.reserve(numWordsExpected);
		return _paragraphs.back();
	}

	inline size_t firstFragmentNumber() const
	{
		return _paragraphs.front().firstFragmentNumber();
	}

	inline size_t lastFragmentNumber() const
	{
		return _paragraphs.back().lastFragmentNumber();
	}

	inline size_t paragraphsCount() const
	{
		return _paragraphs.size();
	}

	QString name;
	std::vector<Paragraph> _paragraphs;
};

class CStructuredText
{
public:
	CStructuredText();

	void addChapter(const QString& name, const std::vector<Paragraph>& paragraphs);
	void addChapter(const Chapter& chapter);
	inline Chapter& addEmptyChapter(const QString& name, size_t expectedNumParagraphs = 100)
	{
		_chapters.emplace_back();
		_chapters.back().name = name;
		_chapters.back()._paragraphs.reserve(expectedNumParagraphs);
		return _chapters.back();
	}

	void clear();

	inline Chapter& lastChapter()
	{
		return _chapters.back();
	}

	inline Paragraph& lastParagraph()
	{
		return lastChapter()._paragraphs.back();
	}

	inline const std::vector<Chapter>& chapters() const
	{
		return _chapters;
	}

	inline size_t chaptersCount() const
	{
		return _chapters.size();
	}

	inline bool empty() const
	{
		return _chapters.empty() || _chapters.front()._paragraphs.empty() || _chapters.front()._paragraphs.front()._fragments.empty();
	}

	inline size_t totalFragmentsCount() const
	{
		return empty() ? 0 : _chapters.back().lastFragmentNumber() + 1;
	}

	const TextFragment& fragment(size_t fragmentIndex) const;

private:
	std::vector<Chapter> _chapters;
};
