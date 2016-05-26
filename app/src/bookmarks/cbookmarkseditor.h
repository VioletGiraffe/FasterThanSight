#pragma once

#include "compiler/compiler_warnings_control.h"
#include "cbookmark.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

#include <deque>

class QShortcut;

namespace Ui {
class CBookmarksEditor;
}

class CBookmarksEditor : public QDialog
{
public:
	CBookmarksEditor(const std::deque<CBookmark>& bookmarks, QWidget *parent);
	~CBookmarksEditor();

	const std::deque<CBookmark>& bookmarks() const;

private:
	void updateUi();
	void updateBookmarksList();

private:
	Ui::CBookmarksEditor *ui;

	std::deque<CBookmark> _bookmarks;

	QShortcut* _deleteBookmarkShortcut;
};
