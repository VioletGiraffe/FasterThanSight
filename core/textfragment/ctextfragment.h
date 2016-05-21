#pragma once

#include <QString>

class TextFragment
{
public:
	enum Delimiter {
		NoDelimiter,
		Space,
		Comma,
		Point,
		ExclamationMark,
		QuestionMark,
		Dash,
		Colon,
		Semicolon,
		Ellipsis,
		Bracket,
		Quote,
		Newline
	};

	inline TextFragment() {};
	TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter = NoDelimiter);

	QString word() const;
	QString punctuation() const;
	QString entireTextFragment() const;
	Delimiter delimiter() const;

	int pivotLetterIndex() const;

private:
	QString _word;
	QString _punctuationText;
	Delimiter _delimitier = NoDelimiter;
};
