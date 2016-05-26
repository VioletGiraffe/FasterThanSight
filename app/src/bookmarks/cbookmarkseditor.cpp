#include "cbookmarkseditor.h"
#include "ui_cbookmarkseditor.h"

DISABLE_COMPILER_WARNINGS
#include <QShortcut>
#include <QTreeWidgetItem>
RESTORE_COMPILER_WARNINGS

enum {ColumnFilePath, ColumnWordNumber, ColumnCount};

CBookmarksEditor::CBookmarksEditor(const std::deque<CBookmark>& bookmarks, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CBookmarksEditor),
	_bookmarks(bookmarks)
{
	ui->setupUi(this);
	updateUi();

	_deleteBookmarkShortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->list);
	connect(_deleteBookmarkShortcut, &QShortcut::activated, [this](){
		for (auto& item: ui->list->selectedItems())
			delete item;

		updateBookmarksList();
	});
}

const std::deque<CBookmark>& CBookmarksEditor::bookmarks() const
{
	return _bookmarks;
}

CBookmarksEditor::~CBookmarksEditor()
{
	delete ui;
}

void CBookmarksEditor::updateUi()
{
	while (ui->list->topLevelItemCount() > 0)
		ui->list->takeTopLevelItem(ui->list->topLevelItemCount() - 1);

	QList<QTreeWidgetItem*> items;
	for (size_t i = 0; i < _bookmarks.size(); ++i)
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(QStringList{_bookmarks[i].filePath, QString::number(_bookmarks[i].wordIndex + 1)}); // +1 is conversion from 0-based index to human-readable 1-based natural number
		item->setData(ColumnFilePath, Qt::UserRole, _bookmarks[i].filePath);
		item->setData(ColumnWordNumber, Qt::UserRole, _bookmarks[i].wordIndex);
		items.push_back(item); 
	}

	ui->list->addTopLevelItems(items);
	ui->list->resizeColumnToContents(ColumnFilePath);
}

void CBookmarksEditor::updateBookmarksList()
{
	_bookmarks.clear();
	for (int i = 0; i < ui->list->topLevelItemCount(); ++i)
	{
		const QTreeWidgetItem* item = ui->list->topLevelItem(i);
		_bookmarks.emplace_back(item->data(ColumnFilePath, Qt::UserRole).toString(), (size_t)item->data(ColumnWordNumber, Qt::UserRole).toULongLong());
	}
}