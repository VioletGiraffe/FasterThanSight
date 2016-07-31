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

	bool eventFilter(QObject* o, QEvent* e) override;

private:
	void initToolBars();
	void initActions();
	void initStatusBar();

	void openBookmark(const CBookmark& bookmark);
	void openFile(const QString& filePath, size_t position);

	void keepScreenFromTurningOff(bool keepFromTurningOff);

	void updateBookmarksMenuItemsList();
	void updateRecentFilesMenu();

	void toggleFullScreen();
	void setFullScreen(bool fullScreen);

	void settingsChanged();

	// CController signal handlers
	void onDisplayUpdateRequired(QString text, bool showPivot, int pivotCharacterIndex);

	void onChapterProgressUpdated(int progressPercentage, QString chapterProgressDescription);
	void onGlobalProgressDescriptionUpdated(QString progressDescription);

	void onReaderStateChanged(CReader::State state);

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
	QLabel* _progressLabel = nullptr;

// Settings
	CThemesDialog _colorsSetupDialog;
};

