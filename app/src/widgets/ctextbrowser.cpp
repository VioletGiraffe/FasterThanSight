#include "ctextbrowser.h"
#include "text/cstructuredtext.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextbrowser.h"
RESTORE_COMPILER_WARNINGS

CTextBrowser::CTextBrowser(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CTextBrowser)
{
	ui->setupUi(this);
}

CTextBrowser::~CTextBrowser()
{
	delete ui;
}

void CTextBrowser::loadText(const CStructuredText& text)
{
	ui->_textView-> setPlainText(text.reconstructText(true));

	for (const Chapter& chapter: text.chapters())
		ui->_chaptersList->addItem(chapter.name);
}
