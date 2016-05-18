#pragma once

#include <QString>

struct TextFragment
{
	enum Delimiter {
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
		Newline
	};

	inline TextFragment(const QString& text, Delimiter delimiter) : _text(text), _delimitier(delimiter) {}

	const QString _text;
	const Delimiter _delimitier;

	TextFragment& operator=(const TextFragment&) = delete;
};
