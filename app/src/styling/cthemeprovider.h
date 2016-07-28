#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QColor>
#include <QObject>
#include <QString>
RESTORE_COMPILER_WARNINGS

#include <deque>
#include <utility>

class CThemeProvider : public QObject
{
	Q_OBJECT

public:
	struct Theme {
		explicit Theme(const QString& str);

		QString _name;

		QColor _windowBgColor;
		QColor _textBgColor;
		QColor _textColor;
		QColor _pivotColor;

		QString toString() const;
	};

	static CThemeProvider& instance();

	size_t currentThemeIndex() const;
	void setCurrentThemeIndex(size_t index);
	size_t acceptedThemeIndex() const;

	Theme& currentTheme();
	const Theme& currentTheme() const;
	const Theme& acceptedTheme() const;

	const std::deque<Theme>& currentThemes() const;
	static std::pair<std::deque<Theme>, size_t> acceptedThemes();

	void addNewTheme(const Theme& theme);
	void deleteCurrentTheme();

	void notifyThemeChanged();
	void saveThemes() const;

signals:
	void currentThemeChanged();

private:
	CThemeProvider();

private:
	std::deque<Theme> _themes;
	size_t _currentThemeIndex = 0;
};

