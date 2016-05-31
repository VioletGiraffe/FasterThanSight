#pragma once

#include "bookmarks/cbookmark.h"
#include "reader/creader.h"
#include "compiler/compiler_warnings_control.h"
#include "styling/ccolorsdialog.h"
#include "recentFiles/crecentfileslist.h"

DISABLE_COMPILER_WARNINGS
#include <QMainWindow>
RESTORE_COMPILER_WARNINGS

#include <deque>

class QLabel;
class QSlider;
class QSpinBox;
class QToolBar;

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow, private CReader::ReaderInterface
{
public:
	explicit CMainWindow(QWidget *parent = 0);
	~CMainWindow();

protected:
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dropEvent(QDropEvent * event) override;

private:
	void initToolBars();
	void initActions();
	void initStatusBar();

	void updateProgressLabel();
	void openFile(const QString& filePath);

	void keepScreenFromTurningOff(bool keepFromTurningOff);

	void loadBookmarksFromSettings();
	void saveBookmarksToSettings() const;
	void updateBookmarksMenuItemsList();
	void updateRecentFilesMenu();

private:
// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void stateChanged(const CReader::State newState) override;

private:
	Ui::CMainWindow *ui;

// Reading settings controls
	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider*  _textSizeSlider = nullptr;

	QSlider*  _readingSpeedSlider = nullptr;
	QSpinBox* _readingSpeedSpinBox = nullptr;

// Status bar widgets
	QLabel* _progressLabel = nullptr;

// Reader
	CReader _reader;

// Bookmarks
	std::deque<CBookmark> _bookmarks;
	CRecentFilesList _recentFiles;

// Settings
	CColorsDialog _colorsSetupDialog;
};

