#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QColor>
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CColorsDialog;
}

class QToolButton;

class CColorsDialog : public QDialog
{
public:
	explicit CColorsDialog(QWidget *parent = 0);
	~CColorsDialog();

	void accept() override;
	void reject() override;

	static QString storedStyle();

private:
	QString temporaryStyle() const;

private:
	void initColorPicker(QToolButton* btn, QColor& color);

private:
	Ui::CColorsDialog *ui;

	QColor _windowBgColor;
	QColor _textBgColor;
	QColor _textColor;
	QColor _pivotColor;
};

