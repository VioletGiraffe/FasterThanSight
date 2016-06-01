#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QColor>
#include <QDialog>
#include <QString>
RESTORE_COMPILER_WARNINGS

#include <deque>
#include <utility>

namespace Ui {
class CColorsDialog;
}

class QToolButton;

class CThemesDialog : public QDialog
{
	struct Theme {
		explicit Theme(const QString& str);

		QString _name;

		QColor _windowBgColor;
		QColor _textBgColor;
		QColor _textColor;
		QColor _pivotColor;

		QString toString() const;
		QString style() const;
	};

public:
	explicit CThemesDialog(QWidget *parent = 0);
	~CThemesDialog();

	static QString currentAcceptedStyle();

private:
	QString currentStyle() const;

	static std::pair<std::deque<Theme>, size_t> themesFromSettings();
	void saveThemes() const;

	void initColorPicker(QToolButton* btn, QColor& color);

private:
	Ui::CColorsDialog *ui;

	std::deque<Theme> _themes;
	size_t _currentThemeIndex = 0;
};

