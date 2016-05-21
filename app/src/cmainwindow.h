#pragma once

#include "reader/creader.h"

#include <QGraphicsOpacityEffect>
#include <QMainWindow>

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

private:
// Reader callbacks
	void updateDisplay(const size_t currentTextFragmentIndex) override;
	void stateChanged(const CReader::State newState) override;

	void updateReadingAnimationDuration();

private:
	Ui::CMainWindow *ui;

	QToolBar* _readingSettingsToolbar = nullptr;
	QSlider*  _textSizeSlider = nullptr;

	QSlider*  _readingSpeedSlider = nullptr;
	QSpinBox* _readingSpeedSpinBox = nullptr;

	CReader _reader;

	QPropertyAnimation* _textFadeOutAnimation = nullptr;
	QGraphicsOpacityEffect _textFadeEffect;
};

