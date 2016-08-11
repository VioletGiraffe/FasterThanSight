#pragma once

#include "ccontroller.h"
#include "styling/cthemesdialog.h"

DISABLE_COMPILER_WARNINGS
#include <QMainWindow>
RESTORE_COMPILER_WARNINGS

class QLabel;
class QProgressBar;
class QSlider;
class QSpinBox;
class QToolBar;

class CClickableLabel;

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
public:
	explicit CMainWindow(QWidget *parent = 0);
	~CMainWindow();

protected:
	void showEvent(QShowEvent *event) override;
	void closeEvent(QCloseEvent * event) override;

	void dragEnterEvent(QDragEnterEvent * event) override;
	void dropEvent(QDropEvent * event) override;

	void wheelEvent(QWheelEvent *event) override;

	bool event(QEvent* e) override;

	bool eventFilter(QObject* o, QEvent* e) override;

private:
	void initToolBars();
	void initActions();
	void initStatusBar();

	void keepScreenFromTurningOff(bool keepFromTurningOff);

	void updateBookmarksMenuItemsList();
	void updateRecentFilesMenu();

	void toggleFullScreen();
	void setFullScreen(bool fullScreen);

	// CController signal handlers
	void onDisplayUpdateRequired(QString text, bool showPivot, int pivotCharacterIndex);

	void onChapterProgressUpdated(int progressPercentage, QString chapterProgressDescription);
	void onGlobalProgressDescriptionUpdated(QString progressDescription);

	void onReaderStateChanged(CReader::State state);

	void onFileOpened(bool success, QString shortFileName);

private:
	Ui::CMainWindow *ui;

	CController _controller;

	QToolBar* _defaultToolbar = nullptr;
// Reading settings controls
	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider*  _brightnessSlider = nullptr;
	QSlider*  _textSizeSlider = nullptr;

	QSlider*  _readingSpeedSlider = nullptr;
	QSpinBox* _readingSpeedSpinBox = nullptr;

// Status bar widgets
	QProgressBar * _chapterProgressBar = nullptr;
	CClickableLabel* _progressLabel = nullptr;
	enum ProgressDisplayMode { Chapter, Book } _statusBarDisplayMode = Book;

// Settings
	CThemesDialog _colorsSetupDialog;
};

