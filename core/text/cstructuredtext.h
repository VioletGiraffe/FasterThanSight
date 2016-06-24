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
	Chapter& addEmptyChapter(const QString& name, size_t expectedNumParagraphs = 100);

	void clear();
	bool empty() const;

	Chapter& lastChapter();
	Paragraph& lastParagraph();

	size_t previousChapterStartIndex(size_t currentIndex) const;
	size_t previousParagraphStartIndex(size_t currentIndex) const;
	size_t nextParagraphStartIndex(size_t currentIndex) const;
	size_t nextChapterStartIndex(size_t currentIndex) const;

	const std::vector<Chapter>& chapters() const;
	size_t chaptersCount() const;
	size_t totalFragmentsCount() const;

	const TextFragment& fragment(size_t fragmentIndex) const;

private:
	std::vector<Chapter> _chapters;
};
