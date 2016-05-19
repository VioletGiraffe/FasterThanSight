#pragma once

#include "reader/creader.h"

#include <QMainWindow>

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

private:
	void initToolBars();
	void initActions();

private:
// Reader callbacks
	void displayText(const TextFragment& text) override;
	void stateChanged(const CReader::State newState) override;

private:
	Ui::CMainWindow *ui;

	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider*  _textSizeSlider = nullptr;

	QSlider*  _readingSpeedSlider = nullptr;
	QSpinBox* _readingSpeedSpinBox = nullptr;

	CReader _reader;
};

