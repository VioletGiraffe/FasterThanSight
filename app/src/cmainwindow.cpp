#include "cmainwindow.h"
#include "compiler/compiler_warnings_control.h"
#include "assert/advanced_assert.h"

#include "widgets/creaderview.h"
#include "bookmarks/cbookmarkseditor.h"

#include "settings/csettings.h"
#include "uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_cmainwindow.h"

#include <QDateTime>
#include <QDebug>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMimeData>
#include <QSlider>
#include <QSpinBox>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStringBuilder>
#include <QToolBar>

#ifdef _WIN32
#include <Windows.h>
#endif

RESTORE_COMPILER_WARNINGS

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow),
	_reader(this),
	_colorsSetupDialog(this)
{
	ui->setupUi(this);

	// Status bar should be inited first so that the rest of the init code can call updateProgressLabel and such
	initStatusBar();
	initToolBars();
	initActions();

	loadBookmarksFromSettings();

	setUnifiedTitleAndToolBarOnMac(true);
	setAcceptDrops(true);
}

CMainWindow::~CMainWindow()
{
	_reader.resetAndStop();

	delete ui;
	ui = nullptr;
}

void CMainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void CMainWindow::dropEvent(QDropEvent *event)
{
	const auto urls = event->mimeData()->urls();
	if (!urls.empty())
		openFile(urls.front().toLocalFile());
}

void CMainWindow::initToolBars()
{
// Reading settings toolbar
	// Font size
	_textSizeSlider = new QSlider(Qt::Horizontal);
	_textSizeSlider->setMinimum(20);
	_textSizeSlider->setMaximum(300);
	_textSizeSlider->setValue(CSettings().value(UI_FONT_SIZE_SETTING, UI_FONT_SIZE_DEFAULT).toInt());

	QFont textFont = ui->_text->font();
	textFont.setFamily(CSettings().value(UI_FONT_FAMILY, textFont.family()).toString());
	textFont.setStyle((QFont::Style)CSettings().value(UI_FONT_STYLE, textFont.style()).toInt());
	textFont.setWeight((QFont::Weight)CSettings().value(UI_FONT_WEIGHT, textFont.weight()).toInt());
	ui->_text->setFont(textFont);

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
		updateProgressLabel(); // To re-calculate remaining reading time based on the new speed
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
			CSettings().setValue(UI_FONT_STYLE, font.style());
			CSettings().setValue(UI_FONT_WEIGHT, font.weight());
			CSettings().setValue(UI_FONT_FAMILY, font.family());
		});
		fontDialog.exec();
	});

	connect(ui->actionShow_pivot, &QAction::triggered, [this](bool checked) {
		CSettings().setValue(UI_SHOW_PIVOT_SETTING, checked);
	});
	ui->actionShow_pivot->setChecked(CSettings().value(UI_SHOW_PIVOT_SETTING, UI_SHOW_PIVOT_DEFAULT).toBool());

	connect(ui->action_Themes, &QAction::triggered, [this]() {
		_colorsSetupDialog.show();
	});

	ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(ui->actionOpen, &QAction::triggered, [this](){
		const QString filePath = QFileDialog::getOpenFileName(this,
			tr("Pick a text file to open"),
			CSettings().value(UI_OPEN_FILE_LAST_USED_DIR_SETTING, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).front()).toString());

		openFile(filePath);
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
		updateProgressLabel();
	});

	connect(ui->actionGo_to_word, &QAction::triggered, [this](){

		const int maxValue = (int)_reader.totalNumWords();
		const int minValue = maxValue > 0 ? 1 : 0;
		bool ok = false;
		const int word = QInputDialog::getInt(this,
			tr("Go to word"),
			tr("Enter the word number to go to, %1 to %2").arg(minValue).arg(maxValue),
			maxValue / 2,
			0,
			maxValue,
			1,
			&ok);

		if (ok)
			_reader.goToWord(word - 1);
	});

	connect(ui->action_Bookmark_current_position, &QAction::triggered, [this](){
		if (_reader.filePath().isEmpty())
			return;

		_bookmarks.emplace_back(_reader.filePath(), _reader.position());
		saveBookmarksToSettings();
		updateBookmarksMenuItemsList();
	});

	connect(ui->action_Remove_bookmarks, &QAction::triggered, [this](){
		CBookmarksEditor editor(_bookmarks, this);
		editor.exec();

		_bookmarks = editor.bookmarks();
		saveBookmarksToSettings();
		loadBookmarksFromSettings(); // to update the menu items list
	});

	connect(ui->action_Exit, &QAction::triggered, qApp, &QApplication::exit);
}

void CMainWindow::initStatusBar()
{
	_progressLabel = new QLabel(this);
	_progressLabel->setAlignment(Qt::AlignRight);
	statusBar()->addWidget(_progressLabel, 1);
}

void CMainWindow::updateProgressLabel()
{
	_progressLabel->setText(
		tr("Reading word %1 out of %2 total (%3%); estimated time remaining: %4")
		.arg(_reader.totalNumWords() > 0 ? _reader.position() + 1 : 0)
		.arg(_reader.totalNumWords())
		.arg(QString::number(100 * (double)_reader.progress(), 'f', 2))
		.arg(QDateTime::fromTime_t(_reader.timeRemainingSeconds()).toUTC().toString("HH:mm:ss"))
	);
}

void CMainWindow::openFile(const QString &filePath)
{
	if (!filePath.isEmpty() && _reader.loadFromFile(filePath))
	{
		CSettings().setValue(UI_OPEN_FILE_LAST_USED_DIR_SETTING, filePath);
		setWindowTitle(qApp->applicationName() % " - " % QFileInfo(filePath).baseName());
		ui->_text->clear();
		updateProgressLabel();
	}
}

void CMainWindow::keepScreenFromTurningOff(bool keepFromTurningOff)
{
#if defined _WIN32
	SetThreadExecutionState(keepFromTurningOff ? (ES_CONTINUOUS | ES_DISPLAY_REQUIRED) : ES_CONTINUOUS);
#elif defined __APPLE__
	#pragma message ("Not implemented")
#else
	#pragma message ("Not implemented")
#endif
}

void CMainWindow::loadBookmarksFromSettings()
{
	_bookmarks.clear();
	const QStringList serializedBookmarks = CSettings().value(UI_BOOKMARKS_STORAGE).toStringList();
	for (const QString& entry: serializedBookmarks)
	{
		const QStringList components = entry.split(';');
		assert_and_return_r(components.size() == 2, );
		_bookmarks.emplace_back(components[0], (size_t)components[1].toULongLong());
	}

	updateBookmarksMenuItemsList();
}

void CMainWindow::saveBookmarksToSettings() const
{
	QStringList serialzedBookmarks;
	for (const CBookmark& bm : _bookmarks)
		serialzedBookmarks.push_back(bm.filePath % ';' % QString::number(bm.wordIndex));

	CSettings().setValue(UI_BOOKMARKS_STORAGE, serialzedBookmarks);
}

void CMainWindow::updateBookmarksMenuItemsList()
{
	for (QAction* action: ui->menu_Bookmarks->actions())
		if (action != ui->action_Bookmark_current_position && action != ui->action_Remove_bookmarks && !action->isSeparator())
			ui->menu_Bookmarks->removeAction(action);

	for (const CBookmark& bm: _bookmarks)
	{
		ui->menu_Bookmarks->addAction(bm.filePath % ": " % QString::number(bm.wordIndex + 1), [this, bm](){
			_reader.loadFromFile(bm.filePath);
			_reader.goToWord(bm.wordIndex);
		});
	}
}

void CMainWindow::updateDisplay(const size_t currentTextFragmentIndex)
{
	const auto& currentFragment = _reader.textFragment(currentTextFragmentIndex);
	const QString text = currentFragment._textFragment.word() + currentFragment._textFragment.punctuation();
	const int pivotCharIndex = ui->actionShow_pivot->isChecked() ? currentFragment._textFragment.pivotLetterIndex() : -1;

	ui->_text->setText(text, pivotCharIndex);

	updateProgressLabel();
}

void CMainWindow::stateChanged(const CReader::State newState)
{
	if (newState == CReader::Reading)
	{
		ui->action_Pause->setEnabled(true);
		keepScreenFromTurningOff(true);
	}
	else if (newState == CReader::Paused)
	{
		ui->action_Pause->setEnabled(false);
		keepScreenFromTurningOff(false);
	}
}
