#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

#include <vector>

class CController;

namespace Ui {
class CTextBrowser;
}

class CStructuredText;

class CTextBrowser : public QDialog
{
public:
	explicit CTextBrowser(QWidget *parent, CController& controller);
	~CTextBrowser();

	void loadText(const CStructuredText& text);

protected:
	bool eventFilter(QObject * o, QEvent * e) override;

private:
	size_t wordIndexForGlobalCoordinates(QPoint pos) const;

private:
	CController& _controller;

	Ui::CTextBrowser *ui;
	std::vector<int> _firstCharacterIndexForFragment;

	QColor _textColor;
	QColor _backgroundColor;
};

