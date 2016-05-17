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
		Bracket
	};

	const QString _text;
	const Delimiter _delimitier;
};
