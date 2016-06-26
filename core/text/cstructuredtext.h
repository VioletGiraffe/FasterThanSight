#pragma once

#include "ctextfragment.h"
#include "container/iterator_helpers.h"

#include <numeric>
#include <vector>

static const size_t paragraphsPerChapter = 120;
static const size_t fragmentsPerParagraph = 100;

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
		_paragraphs.back()._fragments.reserve(fragmentsPerParagraph);
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

	inline size_t wordCount() const
	{
		const size_t count = lastFragmentNumber() - firstFragmentNumber() + 1;
		assert(count == std::accumulate(_paragraphs.cbegin(), _paragraphs.cend(), (size_t)0, [](size_t acc, const Paragraph& p){
			return acc + p._fragments.size();
		}));

		return count;
	}

	QString name;
	std::vector<Paragraph> _paragraphs;
};

class CStructuredText
{
public:
	CStructuredText();
	void setExpectedChaptersCount(size_t count);

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

	struct Stats {
		size_t chapterCount;
		size_t paragraphCount;
		size_t wordCount;

		float avgParagrapsPerChapter;
		size_t maxParagrapsPerChapter;

		float avgWordsPerParagraph;
		size_t maxWordsPerParagraph;
	};

	const Stats stats() const;

	std::vector<Chapter>::const_iterator chapterByWordIndex(size_t index) const;
	std::vector<Paragraph>::const_iterator paragraphByWordIndex(size_t index) const;

private:
	std::vector<IndexedFragment>::const_iterator fragmentByWordIndex(size_t index) const;

private:
	std::vector<Chapter> _chapters;
};
