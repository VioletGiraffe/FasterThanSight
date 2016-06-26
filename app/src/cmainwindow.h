#pragma once

#include "bookmarks/cbookmark.h"
#include "reader/creader.h"
#include "compiler/compiler_warnings_control.h"
#include "styling/cthemesdialog.h"
#include "recentFiles/crecentfileslist.h"

DISABLE_COMPILER_WARNINGS
#include <QMainWindow>
RESTORE_COMPILER_WARNINGS

#include <deque>

class QLabel;
class QProgressBar;
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
	void closeEvent(QCloseEvent * e) override;
	void dragEnterEvent(QDragEnterEvent * event) override;
	void dropEvent(QDropEvent * event) override;

	bool eventFilter(QObject* o, QEvent* e) override;

private:
	void initToolBars();
	void initActions();
	void initStatusBar();

	void openBookmark(const CBookmark& bookmark);
	void openFile(const QString& filePath, size_t position);

	void keepScreenFromTurningOff(bool keepFromTurningOff);

	void loadBookmarksFromSettings();
	void saveBookmarksToSettings() const;
	void updateBookmarksMenuItemsList();
	void updateRecentFilesMenu();

	void toggleFullScreen();

	void settingsChanged();

private:
// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void updateInfo() override;
	void stateChanged(const CReader::State newState) override;

private:
	Ui::CMainWindow *ui;

	QToolBar* _defaultToolbar = nullptr;
// Reading settings controls
	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider*  _brightnessSlider = nullptr;
	QSlider*  _textSizeSlider = nullptr;

	QSlider*  _readingSpeedSlider = nullptr;
	QSpinBox* _readingSpeedSpinBox = nullptr;

// Status bar widgets
	QProgressBar * _chapterProgressBar = nullptr;
	QLabel* _progressLabel = nullptr;

// Reader
	CReader _reader;

// Bookmarks
	std::deque<CBookmark> _bookmarks;
	CRecentFilesList _recentFiles;

// Settings
	CThemesDialog _colorsSetupDialog;
};

