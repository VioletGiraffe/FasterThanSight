#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CColorsDialog;
}

class CColorsDialog : public QDialog
{
public:
	explicit CColorsDialog(QWidget *parent = 0);
	~CColorsDialog();

	void reject() override;

private:
	QString generateStyleSheet();
	void applyStyleSheet();

private:
	Ui::CColorsDialog *ui;
};

