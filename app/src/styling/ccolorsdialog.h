#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QColor>
#include <QDialog>
RESTORE_COMPILER_WARNINGS

#include <deque>

namespace Ui {
class CColorsDialog;
}

class QToolButton;

class CColorsDialog : public QDialog
{
	struct Theme {
		QColor _windowBgColor;
		QColor _textBgColor;
		QColor _textColor;
		QColor _pivotColor;

		QString toString() const;
		void fromString(const QString& str);
	};

public:
	explicit CColorsDialog(QWidget *parent = 0);
	~CColorsDialog();

	void accept() override;
	void reject() override;

	static QString storedStyle();

private:
	QString temporaryStyle() const;

	void loadThemes();
	void initColorPicker(QToolButton* btn, QColor& color);

private:
	Ui::CColorsDialog *ui;

	std::deque<Theme> _themes;
};

