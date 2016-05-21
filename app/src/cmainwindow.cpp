#include "cmainwindow.h"
#include "ui_cmainwindow.h"
#include "compiler/compiler_warnings_control.h"

#include "settings/csettings.h"
#include "uisettings.h"

DISABLE_COMPILER_WARNINGS
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QPropertyAnimation>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QToolBar>
RESTORE_COMPILER_WARNINGS

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow),
	_reader(this)
{
	ui->setupUi(this);

	// Reading animation
	_textFadeEffect.setOpacity(1.0f);
	_textFadeOutAnimation = new QPropertyAnimation(&_textFadeEffect, "opacity", this);
	_textFadeOutAnimation->setStartValue(1.0f);
	_textFadeOutAnimation->setEndValue(0.0f);
	_textFadeOutAnimation->setEasingCurve(QEasingCurve::OutQuad);

	ui->_text->setGraphicsEffect(&_textFadeEffect);
	updateReadingAnimationDuration();
	
// Toolbars
	initToolBars();

// Actions
	initActions();
}

CMainWindow::~CMainWindow()
{
	_reader.resetAndStop();
	_textFadeOutAnimation->stop();

	delete ui;
	ui = nullptr;
}

void CMainWindow::initToolBars()
{
// Reading settings toolbar
	// Font size
	_textSizeSlider = new QSlider(Qt::Horizontal);
	_textSizeSlider->setMinimum(20);
	_textSizeSlider->setMaximum(300);
	_textSizeSlider->setValue(CSettings().value(UI_FONT_SIZE_SETTING, UI_FONT_SIZE_DEFAULT).toInt());

	connect(_textSizeSlider, &QSlider::valueChanged, [this](int size){
		QFont font = ui->_text->font();
		font.setPointSize(size);
		ui->_text->setFont(font);

		CSettings().setValue(UI_FONT_SIZE_SETTING, size);
	});
	_textSizeSlider->valueChanged(_textSizeSlider->value());

	_readingSettingsToolbar = addToolBar(tr("Reading settings"));
	_readingSettingsToolbar->addWidget(new QLabel(tr("Text size") + "  "));
	_readingSettingsToolbar->addWidget(_textSizeSlider);

	// Reading speed
	_readingSettingsToolbar->addSeparator();
	_readingSettingsToolbar->addWidget(new QLabel(tr("Reading speed") + "  "));

	_readingSpeedSlider = new QSlider(Qt::Horizontal);
	_readingSpeedSpinBox = new QSpinBox();

	_readingSpeedSlider->setMinimum(100);
	_readingSpeedSpinBox->setMinimum(100);
	_readingSpeedSlider->setMaximum(800);
	_readingSpeedSpinBox->setMaximum(800);

	_readingSpeedSlider->setTickInterval(50);
	_readingSpeedSlider->setTickPosition(QSlider::TicksBothSides);

	_readingSpeedSpinBox->setSuffix(" WPM");
	_readingSpeedSpinBox->setAccelerated(true);

	connect(_readingSpeedSlider, &QSlider::valueChanged, _readingSpeedSpinBox, &QSpinBox::setValue);
	connect(_readingSpeedSpinBox, (void (QSpinBox::*)(int))&QSpinBox::valueChanged, _readingSpeedSlider, &QSlider::setValue);

	connect(_readingSpeedSlider, &QSlider::valueChanged, [this](int WPM){
		_reader.setReadingSpeed(WPM);
		updateReadingAnimationDuration();
	});

	_readingSpeedSlider->setValue(_reader.readingSpeed());

	_readingSettingsToolbar->addWidget(_readingSpeedSlider);
	_readingSettingsToolbar->addWidget(_readingSpeedSpinBox);
}

void CMainWindow::initActions()
{
	connect(ui->action_Font, &QAction::triggered, [this](){
		QFontDialog fontDialog(ui->_text->font(), this);
		connect(&fontDialog, &QFontDialog::fontSelected, [this](const QFont &font){
			ui->_text->setFont(font);
			_textSizeSlider->setValue(font.pointSize());
		});
		fontDialog.exec();
	});

	connect(ui->actionShow_pivot, &QAction::triggered, [this](bool checked) {
		CSettings().setValue(UI_SHOW_PIVOT_SETTING, checked);
	});
	ui->actionShow_pivot->setChecked(CSettings().value(UI_SHOW_PIVOT_SETTING, UI_SHOW_PIVOT_DEFAULT).toBool());

	ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(ui->actionOpen, &QAction::triggered, [this](){
		const QString filePath = QFileDialog::getOpenFileName(this,
			tr("Pick a text file to open"),
			CSettings().value(UI_OPEN_FILE_LAST_USED_DIR_SETTING, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).front()).toString());

		if (!filePath.isEmpty())
		{
			CSettings().setValue(UI_OPEN_FILE_LAST_USED_DIR_SETTING, filePath);
			setWindowTitle(qApp->applicationName() % " - " % QFileInfo(filePath).baseName());
			ui->_text->clear();
			_reader.loadFromFile(filePath);
		}
	});

	ui->action_Read->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
	connect(ui->action_Read, &QAction::triggered, [this](){
		if (_reader.state() == CReader::Paused)
			_reader.resumeReading();
		else
			_reader.pauseReading();
	});

	ui->action_Pause->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
	connect(ui->action_Pause, &QAction::triggered, [this](){
		_reader.pauseReading();
	});

	ui->actionStop->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
	connect(ui->actionStop, &QAction::triggered, [this](){
		_reader.resetAndStop();
		ui->_text->clear();
	});

	connect(ui->action_Exit, &QAction::triggered, qApp, &QApplication::exit);
}

void CMainWindow::updateDisplay(const size_t currentTextFragmentIndex)
{
	static const auto pivotize = [this](const QString& text, int pivotCharacterIndex) -> QString {
		if (ui->actionShow_pivot->isChecked())
			return text.left(pivotCharacterIndex) % "<font color=\"red\">" % text[pivotCharacterIndex] % "</font>" % text.mid(pivotCharacterIndex + 1);
		else
			return text;
	};

	const auto& currentFragment = _reader.textFragment(currentTextFragmentIndex);
	const QString text = pivotize(currentFragment._textFragment.word(), currentFragment._textFragment.pivotLetterIndex()) + currentFragment._textFragment.punctuation();

	// If the new text fragment equals the previous one, use animation to make it obvious 
	if (ui->_text->text() != text)
		ui->_text->setText(text);
	else
	{
		QMetaObject::Connection* connection = new QMetaObject::Connection();
		*connection = connect(_textFadeOutAnimation, &QPropertyAnimation::finished, [this, text, connection]() {
			disconnect(*connection);
			delete connection;

			_textFadeEffect.setOpacity(1.0f);
			ui->_text->setText(text);
		});

		_textFadeOutAnimation->start();
	}
}

void CMainWindow::stateChanged(const CReader::State newState)
{
	if (newState == CReader::Reading)
	{
		ui->action_Pause->setEnabled(true);
	}
	else if (newState == CReader::Paused)
	{
		ui->action_Pause->setEnabled(false);
	}
}

void CMainWindow::updateReadingAnimationDuration()
{
	const auto animationDurationMs = 60 * 1000 * 2 / _reader.readingSpeed() / 3;
	_textFadeOutAnimation->setDuration(std::min(animationDurationMs, 150U));
}
