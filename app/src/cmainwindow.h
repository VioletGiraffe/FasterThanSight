#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

class QSlider;
class QToolBar;

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
public:
	explicit CMainWindow(QWidget *parent = 0);
	~CMainWindow();

private:
	void initToolBars();
	void initActions();

private:
	Ui::CMainWindow *ui;

	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider * _textSizeSlider;
};

#endif // CMAINWINDOW_H
