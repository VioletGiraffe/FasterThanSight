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
		Quote,
		Newline
	};

	inline TextFragment(const QString& text, Delimiter delimiter) : _text(text), _delimitier(delimiter) {}
	inline TextFragment(const TextFragment& other) : _text(other._text), _delimitier(other._delimitier) {}

	const QString _text;
	const Delimiter _delimitier;

	TextFragment& operator=(const TextFragment&) = delete;
};
