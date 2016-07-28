#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CColorsDialog;
}

class QToolButton;

class CThemesDialog : public QDialog
{
public:
	explicit CThemesDialog(QWidget *parent = 0);
	~CThemesDialog();

protected:
	void showEvent(QShowEvent* e) override;

private:
	void loadThemes();
	void initColorPicker(QToolButton* btn, QColor& color);

	void createNewTheme();
	void deleteTheme();

private:
	Ui::CColorsDialog *ui;
};

