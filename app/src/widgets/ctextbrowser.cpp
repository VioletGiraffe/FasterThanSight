#include "ctextbrowser.h"
#include "text/cstructuredtext.h"
#include "system/ctimeelapsed.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextbrowser.h"

#include <QDebug>
RESTORE_COMPILER_WARNINGS

CTextBrowser::CTextBrowser(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CTextBrowser)
{
	ui->setupUi(this);

	connect(ui->_chaptersList, &QListWidget::itemActivated, [this](const QListWidgetItem* item){
		ui->_textView->moveCursor(QTextCursor::Start);
		QTextCursor cursor = ui->_textView->textCursor();
		CTimeElapsed t(true);
		cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, item->data(Qt::UserRole).toInt());
		qDebug() << t.elapsed();
		ui->_textView->moveCursor(QTextCursor::End); // This ensures that the requested position is at the top of the text view
		ui->_textView->setTextCursor(cursor);
	});
}

CTextBrowser::~CTextBrowser()
{
	delete ui;
}

void CTextBrowser::loadText(const CStructuredText& text)
{
	CTimeElapsed timer(true);
	const auto textStruct = text.reconstructText();
	qDebug() << "reconstructText took" << timer.elapsed() << "ms";

	timer.start();
	ui->_textView-> setPlainText(textStruct.text);
	qDebug() << "Setting text to the display widget took" << timer.elapsed() << "ms";

	for (const Chapter& chapter: text.chapters())
	{
		QListWidgetItem * item = new QListWidgetItem(chapter.name, ui->_chaptersList);
		item->setData(Qt::UserRole, textStruct.firstCharacterIndexForFragment[chapter.firstFragmentNumber()]);
	}
}
