#pragma once

#include "reader/creader.h"

#include <QGraphicsOpacityEffect>
#include <QMainWindow>

class QLabel;
class QSlider;
class QSpinBox;
class QPropertyAnimation;
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
	void initStatusBar();

private:
// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void stateChanged(const CReader::State newState) override;

	void updateReadingAnimationDuration();

	void updateProgressLabel();

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

// UI stuff
	QPropertyAnimation* _textFadeOutAnimation = nullptr;
	QGraphicsOpacityEffect _textFadeEffect;
};

