#pragma once

#include "ctextfragment.h"
#include "container/iterator_helpers.h"

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
	inline Paragraph& addEmptyParagraph()
	{
		_paragraphs.emplace_back();
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
	Chapter& addEmptyChapter(const QString& name);

	void clear();
	bool empty() const;

	void removeEmptyItems();

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

	struct const_iterator {
		friend class CStructuredText;

		const_iterator& operator++();
		const IndexedFragment& operator*() const;
		IndexedFragment const* operator->() const;

		bool operator==(const const_iterator& other) const;
		bool operator!=(const const_iterator& other) const;

	private:
		const_forward_iterator_wrapper<std::vector<Chapter>> _chapterIterator;
		const_forward_iterator_wrapper<std::vector<Paragraph>> _paragraphIterator;
		const_forward_iterator_wrapper<std::vector<IndexedFragment>> _fragmentIterator;
	};

	const_iterator begin() const;
	const_iterator end() const;

private:
	std::vector<Chapter> _chapters;
};
