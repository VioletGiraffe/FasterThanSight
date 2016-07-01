#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CTextBrowser;
}

class CStructuredText;

class CTextBrowser : public QDialog
{
public:
	explicit CTextBrowser(QWidget *parent = 0);
	~CTextBrowser();

	void loadText(const CStructuredText& text);

private:
	Ui::CTextBrowser *ui;
};

