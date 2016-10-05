#include "ctextbrowser.h"
#include "text/cstructuredtext.h"
#include "system/ctimeelapsed.h"
#include "../ccontroller.h"
#include "container/algorithms.hpp"
#include "../styling/cthemeprovider.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ctextbrowser.h"

#include <QDebug>
#include <QMenu>
#include <QScrollBar>
RESTORE_COMPILER_WARNINGS

CTextBrowser::CTextBrowser(QWidget *parent, CController& controller) :
	QDialog(parent),
	_controller(controller),
	ui(new Ui::CTextBrowser)
{
	ui->setupUi(this);

	ui->_textView->verticalScrollBar()->installEventFilter(this);
	ui->_textView->viewport()->installEventFilter(this);
	ui->_textView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->_textView, &QWidget::customContextMenuRequested, this, [this](QPoint pos){

		if (_firstCharacterIndexForFragment.empty())
			return;

		QMenu menu;
		QAction * readFromHereAction = menu.addAction("Read from here");
		if (menu.exec(ui->_textView->mapToGlobal(pos)) == readFromHereAction)
			_controller.goToWord(wordIndexForGlobalCoordinates(ui->_textView->mapToGlobal(pos)));
	});

	connect(ui->_chaptersList, &QListWidget::itemActivated, this, [this](const QListWidgetItem* item){
		ui->_textView->moveCursor(QTextCursor::Start);
		QTextCursor cursor = ui->_textView->textCursor();
		CTimeElapsed t(true);
		cursor.setPosition(item->data(Qt::UserRole).toInt());
		ui->_textView->moveCursor(QTextCursor::End); // This ensures that the requested position is at the top of the text view
		ui->_textView->setTextCursor(cursor);
	});

	const CThemeProvider& themeProvider = CThemeProvider::instance();
	_textColor = themeProvider.currentTheme()._textColor;
	_backgroundColor = themeProvider.currentTheme()._windowBgColor;

	ui->_textView->setStyleSheet("QPlainTextEdit {" + QString("color:%1; background-color:%2;").arg(_textColor.name()).arg(_backgroundColor.name()) + "}");
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
		item->setData(Qt::UserRole, _firstCharacterIndexForFragment[chapter.firstFragmentNumber()]);
	}

	ui->_chaptersList->setFixedWidth(ui->_chaptersList->sizeHintForColumn(0) + qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 10);

	auto cursor = ui->_textView->textCursor();
	cursor.setPosition(_firstCharacterIndexForFragment[_controller.position()]);
	ui->_textView->setTextCursor(cursor);
	ui->_textView->ensureCursorVisible();
}

bool CTextBrowser::eventFilter(QObject * o, QEvent * e)
{
	QWidget * scrollBar = ui->_textView->verticalScrollBar();
	if (o == scrollBar && e->type() == QEvent::Paint)
	{
		const int character = ui->_textView->cursorForPosition({ui->_textView->width() / 2, ui->_textView->height() / 2}).position();
		int currentChapterItemIndex = -1;

		for (int i = ui->_chaptersList->count() - 1; i >= 0; --i)
		{
			const int chapterStartCharacterIndex = ui->_chaptersList->item(i)->data(Qt::UserRole).toInt();
			if (chapterStartCharacterIndex < character)
			{
				currentChapterItemIndex = i;
				break;
			}
		}

		for (int i = 0, count = ui->_chaptersList->count(); i < count; ++i)
		{
			auto item = ui->_chaptersList->item(i);
			QFont f = item->font();
			f.setBold(i == currentChapterItemIndex);
			item->setFont(f);
			if (i == currentChapterItemIndex)
			{
				item->setTextColor(_backgroundColor);
				item->setBackgroundColor(_textColor);
			}
			else
			{
				item->setTextColor(_textColor);
				item->setBackgroundColor(_backgroundColor);
			}
		}

		if (currentChapterItemIndex >= 0)
			ui->_chaptersList->scrollToItem(ui->_chaptersList->item(currentChapterItemIndex));
	}
	else if (o == ui->_textView->viewport() && e->type() == QEvent::MouseButtonDblClick)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(e);
		if (!_firstCharacterIndexForFragment.empty())
		{
			_controller.goToWord(wordIndexForGlobalCoordinates(static_cast<QWidget*>(o)->mapToGlobal(mouseEvent->pos())));
			accept();
		}
	}

	return QDialog::eventFilter(o, e);
}

size_t CTextBrowser::wordIndexForGlobalCoordinates(QPoint pos) const
{
	const int c = ui->_textView->cursorForPosition(ui->_textView->mapFromGlobal(pos)).position();
	auto indexItem = std::lower_bound(_firstCharacterIndexForFragment.begin(), _firstCharacterIndexForFragment.end(), c);
	if (indexItem != _firstCharacterIndexForFragment.begin())
		--indexItem;

	const auto wordIndex = indexItem - _firstCharacterIndexForFragment.begin();
	return (size_t)wordIndex;
}
