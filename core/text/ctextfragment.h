#pragma once
#include "compiler/compiler_warnings_control.h"
#include "lang/enum.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

class TextFragment
{
public:
	// Ordered by ascending priority!
	enum DelimiterEnum {
		NoDelimiter,
		Quote, // Quote must have low priority as to not override punctuation marks that denote the end of a sentence. Else the sentence end cannot be detected (#41)
		Space,
		Dash,
		Bracket,
		Comma,
		Colon,
		Semicolon,
		Point,
		Ellipsis,
		ExclamationMark,
		QuestionMark,
		Newline // Must have the highest priority for correct paragraph parsing. Requires the source text to be well-formed.
				// Newline only be used for semantical formatting, not for limiting the visual line length.
	};

	typedef Enum<DelimiterEnum> Delimiter;

	enum PivotCalculationMethod {
		pcmMagic,

		pcmMiddle,
		pcmQuarter,
		pcmSquareRoot,
		pcmCubicRoot,
		pcmLogarithm
	};

	inline TextFragment() {}
	TextFragment(const QString& word, const QString& punctuation, Delimiter delimiter = NoDelimiter);

	// Word + possible punctuation following it
	QString text() const;
	// Just the word
	QString word() const;
	// Just the punctuation
	QString punctuation() const;
	Delimiter delimiter() const;

	int pivotLetterIndex(PivotCalculationMethod method) const;
	bool isEndOfSentence() const;
	bool isEmpty() const;

private:
	QString _word;
	QString _punctuationText;
	Delimiter _delimitier = NoDelimiter;
};
