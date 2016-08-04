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

	CThemeProvider(); // The default constructor cannot be private because then it would be impossible to register this type in QML
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

	// Helper methods for styling the QML UI
	Q_INVOKABLE QColor pivotColor() const;
	Q_INVOKABLE QColor backgroundColor() const;
	Q_INVOKABLE QColor textColor() const;

signals:
	void currentThemeChanged();

private:
	std::deque<Theme> _themes;
	size_t _currentThemeIndex = 0;
};

