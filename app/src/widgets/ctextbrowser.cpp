#include "ctextbrowser.h"
#include "text/cstructuredtext.h"
#include "system/ctimeelapsed.h"
#include "reader/creader.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextbrowser.h"

#include <QDebug>
#include <QMenu>
RESTORE_COMPILER_WARNINGS

CTextBrowser::CTextBrowser(QWidget *parent, CReader& reader) :
	QDialog(parent),
	_reader(reader),
	ui(new Ui::CTextBrowser)
{
	ui->setupUi(this);

	ui->_textView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->_textView, &QWidget::customContextMenuRequested, [this](QPoint pos){

		if (_firstCharacterIndexForFragment.empty())
			return;

		QMenu menu;
		QAction * readFromHereAction = menu.addAction("Read from here");
		QAction * result = menu.exec(ui->_textView->mapToGlobal(pos));
		if (result == readFromHereAction)
		{
			const int c = ui->_textView->cursorForPosition(pos).position();
			auto indexItem = std::lower_bound(_firstCharacterIndexForFragment.begin(), _firstCharacterIndexForFragment.end(), c);
			if (indexItem != _firstCharacterIndexForFragment.begin())
				--indexItem;

			const auto wordIndex = indexItem - _firstCharacterIndexForFragment.begin();
			_reader.goToWord(wordIndex);
		}
	});

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
	_firstCharacterIndexForFragment = textStruct.firstCharacterIndexForFragment;
	qDebug() << "reconstructText took" << timer.elapsed() << "ms";

	timer.start();
	ui->_textView-> setPlainText(textStruct.text);
	qDebug() << "Setting text to the display widget took" << timer.elapsed() << "ms";

	for (const Chapter& chapter: text.chapters())
	{
		QListWidgetItem * item = new QListWidgetItem(chapter.name, ui->_chaptersList);
		item->setData(Qt::UserRole, textStruct.firstCharacterIndexForFragment[chapter.firstFragmentNumber()]);
	}

	ui->_chaptersList->setFixedWidth(ui->_chaptersList->sizeHintForColumn(0) + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 10);
}
