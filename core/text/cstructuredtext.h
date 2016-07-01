#pragma once

#include "ctextfragment.h"
#include "container/iterator_helpers.h"
#include "cchapter.h"

#include <vector>

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

	QString reconstructText(bool richText = false) const;

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
