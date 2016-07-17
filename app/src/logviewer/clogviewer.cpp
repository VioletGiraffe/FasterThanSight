#include "clogviewer.h"
#include "../logger.h"

DISABLE_COMPILER_WARNINGS
#include "ui_clogviewer.h"
RESTORE_COMPILER_WARNINGS

CLogViewer::CLogViewer(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CLogViewer)
{
	ui->setupUi(this);

	ui->_logText->moveCursor(QTextCursor::End);
	for (const QString& line: loggerInstance().contents())
	{
		ui->_logText->insertPlainText(line + '\n');
		ui->_logText->moveCursor(QTextCursor::End);
	}

	ui->_logText->moveCursor(QTextCursor::StartOfLine);
}

CLogViewer::~CLogViewer()
{
	delete ui;
}
