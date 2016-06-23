#pragma once

#include "ctextfragment.h"

#include <vector>

struct IndexedFragment {
	TextFragment fragment;
	size_t fragmentIndex;
};

struct Paragraph {
	inline size_t firstFragmentNumber() const {
		return _fragments.front().fragmentIndex;
	}

	inline size_t lastFragmentNumber() const {
		return _fragments.back().fragmentIndex;
	}

	std::vector<const IndexedFragment> _fragments;
};

struct Chapter {
	inline size_t firstFragmentNumber() const {
		return _paragraphs.front().firstFragmentNumber();
	}

	inline size_t lastFragmentNumber() const {
		return _paragraphs.back().lastFragmentNumber();
	}

	QString name;
	std::vector<const Paragraph> _paragraphs;
};

class CStructuredText
{
public:
	CStructuredText();

	void addChapter(const QString& name, const std::vector<const Paragraph>& paragraphs);

	inline const std::vector<const Chapter>& chapters() const
	{
		return _chapters;
	}

	inline size_t numFragments() const
	{
		return _chapters.back().lastFragmentNumber() + 1;
	}

	const TextFragment& fragment(size_t fragmentIndex) const;

private:
	std::vector<const Chapter> _chapters;
};
