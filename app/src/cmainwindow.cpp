#include "cmainwindow.h"
#include "compiler/compiler_warnings_control.h"
#include "assert/advanced_assert.h"
#include "updaterUI/cupdaterdialog.h"
#include "version.h"
#include "uihelpers.h"
#include "aboutdialog/caboutdialog.h"

#include "widgets/creaderview.h"
#include "widgets/ctextbrowser.h"
#include "bookmarks/cbookmarkseditor.h"

#include "settings/csettings.h"
#include "settingsui/csettingsdialog.h"
#include "settings/csettingspagepivot.h"
#include "settings/csettingspagepauses.h"
#include "settings/csettingspageinterface.h"
#include "settings/csettingspagetools.h"
#include "uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_cmainwindow.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QShortcut>
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

#define WINDOW_GEOMETRY QStringLiteral("UI/Window/Geometry")
#define WINDOW_STATE QStringLiteral("UI/Window/State")

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow),
	_reader(this),
	_colorsSetupDialog(this)
{
	ui->setupUi(this);

	setUnifiedTitleAndToolBarOnMac(true);
	setAcceptDrops(true);

	ui->_text->installEventFilter(this);

	_reader.setClearScreenAfterSentenceEnd(CSettings().value(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, UI_CLEAR_SCREEN_AFTER_SENTENCE_END_DEFAULT).toBool());

	// Status bar should be inited first so that the rest of the init code can call updateProgressLabel and such
	initStatusBar();
	initToolBars();
	initActions();

	loadBookmarksFromSettings();
	updateRecentFilesMenu();
	if (!_recentFiles.items().empty())
	{
		const CBookmark& lastBookmark = _recentFiles.items().front();
		openBookmark(lastBookmark);
	}

	settingsChanged();
}

CMainWindow::~CMainWindow()
{
	_reader.pauseReading();
	if (!_reader.filePath().isEmpty())
		_recentFiles.updateWith(_reader.filePath(), _reader.position());

	delete ui;
	ui = nullptr;
}

void CMainWindow::showEvent(QShowEvent *event)
{
	CSettings s;
	restoreGeometry(s.value(WINDOW_GEOMETRY).toByteArray());
	restoreState(s.value(WINDOW_STATE).toByteArray());
	setFullScreen(isFullScreen()); // restoreState itself will make the window full screen, but we may need additional logic like hiding menu bar and tool bars

	QMainWindow::showEvent(event);
}

void CMainWindow::closeEvent(QCloseEvent * event)
{
	CSettings s;
	s.setValue(WINDOW_STATE, saveState());
	s.setValue(WINDOW_GEOMETRY, saveGeometry());

	QMainWindow::closeEvent(event);
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
		openFile(urls.front().toLocalFile(), 0);
}

void CMainWindow::wheelEvent(QWheelEvent *event)
{
	if ((event->modifiers() & Qt::ControlModifier) && event->angleDelta().y() != 0)
	{
		event->accept();
		qDebug() << event->angleDelta();
		const int delta = event->angleDelta().y() / 8 / 15;
		_textSizeSlider->setValue(_textSizeSlider->value() + delta * 10);
	}
	else
		QMainWindow::wheelEvent(event);
}

bool CMainWindow::eventFilter(QObject* /*o*/, QEvent* e)
{
	switch (e->type())
	{
	case QEvent::MouseButtonRelease:
		ui->_text->setFocus();
		_reader.togglePause();
		break;
	case QEvent::MouseButtonDblClick:
		toggleFullScreen();
		break;
	default:
		break;
	}

	return false;
}

void CMainWindow::initToolBars()
{
	CSettings s;
// Main (default) toolbar

	QList<QToolBar*> toolbars = findChildren<QToolBar*>();
	assert_r(toolbars.size() == 1);
	_defaultToolbar = toolbars.front();

// Reading settings toolbar
	_readingSettingsToolbar = addToolBar(tr("Reading settings"));
	_readingSettingsToolbar->setObjectName("_readingSettingsToolbar");

	// Screen brightness
	_brightnessSlider = new QSlider(Qt::Horizontal);
	_brightnessSlider->setMinimum(1);
	_brightnessSlider->setMaximum(100);
	connect(_brightnessSlider, &QSlider::valueChanged, [](int value){
		CSettings().setValue(UI_BRIGHTNESS, value);
		qApp->setStyleSheet(CThemesDialog::currentAcceptedStyle());
	});
	_brightnessSlider->setValue(s.value(UI_BRIGHTNESS, UI_BRIGHTNESS_DEFAULT).toInt());
	_readingSettingsToolbar->addWidget(new QLabel(tr("Brightness") + "  "));
	_readingSettingsToolbar->addWidget(_brightnessSlider);
	_readingSettingsToolbar->addSeparator();

	// Font size
	_textSizeSlider = new QSlider(Qt::Horizontal);
	_textSizeSlider->setMinimum(20);
	_textSizeSlider->setMaximum(300);
	connect(_textSizeSlider, &QSlider::valueChanged, [this](int size) {
		QFont font = ui->_text->font();
		font.setPointSize(size);
		ui->_text->setFont(font);

		CSettings().setValue(UI_FONT_SIZE_SETTING, size);
	});
	_textSizeSlider->setValue(s.value(UI_FONT_SIZE_SETTING, UI_FONT_SIZE_DEFAULT).toInt());

	QFont textFont = ui->_text->font();
	textFont.setFamily(s.value(UI_FONT_FAMILY, textFont.family()).toString());
	textFont.setStyle((QFont::Style)s.value(UI_FONT_STYLE, textFont.style()).toInt());
	textFont.setWeight((QFont::Weight)s.value(UI_FONT_WEIGHT, textFont.weight()).toInt());
	ui->_text->setFont(textFont);

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
	connect(_readingSpeedSpinBox, &QSpinBox::editingFinished, [this](){
		_readingSpeedSlider->setValue(_readingSpeedSpinBox->value());
		ui->_text->setFocus();
	});

	connect(_readingSpeedSlider, &QSlider::valueChanged, [this](int WPM){
		_reader.setReadingSpeed(WPM);
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
		_reader.updateInterface();
	});
	ui->actionShow_pivot->setChecked(CSettings().value(UI_SHOW_PIVOT_SETTING, UI_SHOW_PIVOT_DEFAULT).toBool());

	connect(ui->actionClear_screen_after_sentence_end, &QAction::triggered, [this](bool checked) {
		CSettings().setValue(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, checked);
		_reader.setClearScreenAfterSentenceEnd(checked);

		QMessageBox::information(this, QString(), tr("The file must be reloaded for the change to take effect."));
	});
	ui->actionClear_screen_after_sentence_end->setChecked(CSettings().value(UI_CLEAR_SCREEN_AFTER_SENTENCE_END, UI_CLEAR_SCREEN_AFTER_SENTENCE_END_DEFAULT).toBool());

	connect(ui->action_Themes, &QAction::triggered, [this]() {
		_colorsSetupDialog.show();
	});

	ui->actionOpen->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirOpenIcon));
	connect(ui->actionOpen, &QAction::triggered, [this](){
		const QString filePath = QFileDialog::getOpenFileName(this,
			tr("Pick a text file to open"),
			CSettings().value(UI_OPEN_FILE_LAST_USED_DIR_SETTING, QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).front()).toString());

		openFile(filePath, 0);
	});

	ui->action_Read->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
	connect(ui->action_Read, &QAction::triggered, [this](){
		_reader.togglePause();
	});

	ui->action_Pause->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
	connect(ui->action_Pause, &QAction::triggered, [this](){
		_reader.pauseReading();
	});

	ui->actionStop->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
	connect(ui->actionStop, &QAction::triggered, [this](){
		_reader.resetAndStop();
		ui->_text->clear();
	});

	ui->actionPrevious_chapter->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipBackward));
	connect(ui->actionPrevious_chapter, &QAction::triggered, [this](){
		_reader.toPreviousChapter();
	});

	ui->actionPrevious_paragraph->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSeekBackward));
	connect(ui->actionPrevious_paragraph, &QAction::triggered, [this](){
		_reader.toPreviousParagraph();
	});

	ui->actionNext_paragraph->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSeekForward));
	connect(ui->actionNext_paragraph, &QAction::triggered, [this](){
		_reader.toNextParagraph();
	});

	ui->actionNext_chapter->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaSkipForward));
	connect(ui->actionNext_chapter, &QAction::triggered, [this](){
		_reader.toNextChapter();
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

#ifndef __APPLE__
	auto f11 = new QShortcut(QKeySequence("F11"), this);
	connect(f11, &QShortcut::activated, this, &CMainWindow::toggleFullScreen);
	connect(ui->action_Fullscreen, &QAction::triggered, this, &CMainWindow::toggleFullScreen);
#else
	ui->action_Fullscreen->setVisible(false);
#endif

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

	connect(ui->actionView_navigate_text, &QAction::triggered, [this](){
		CTextBrowser browser(this, _reader);
		browser.loadText(_reader.text());
		browser.exec();
	});

	connect(ui->actionSettings, &QAction::triggered, [this](){
		CSettingsDialog settingsDialog(this);
		settingsDialog
				.addSettingsPage(new CSettingsPageInterface)
				.addSettingsPage(new CSettingsPagePivot)
				.addSettingsPage(new CSettingsPagePauses)
				.addSettingsPage(new CSettingsPageTools);

		connect(&settingsDialog, &CSettingsDialog::settingsChanged, this, &CMainWindow::settingsChanged);

		settingsDialog.exec();
	});

	connect(ui->actionCheck_for_updates, &QAction::triggered, [this](){
		CUpdaterDialog(this, REPO_ADDRESS, VERSION_STRING).exec();
	});

	connect(ui->actionAbout, &QAction::triggered, [this](){
		CAboutDialog(VERSION_STRING, this).exec();
	});
}

void CMainWindow::initStatusBar()
{
	QStatusBar * bar = statusBar();
	_chapterProgressBar = new QProgressBar(this);
	_chapterProgressBar->setVisible(false);
	_chapterProgressBar->setTextVisible(false);
	bar->addWidget(_chapterProgressBar, 100);

	_progressLabel = new QLabel(this);
	_progressLabel->setAlignment(Qt::AlignRight);
	bar->addWidget(_progressLabel, 1);
}

void CMainWindow::openBookmark(const CBookmark& bookmark)
{
	openFile(bookmark.filePath, bookmark.wordIndex);
}

void CMainWindow::openFile(const QString &filePath, size_t position)
{
	const CBookmark lastPosition(_reader.filePath(), _reader.position());
	if (!filePath.isEmpty() && _reader.loadFromFile(filePath))
	{
		// Opening a new file - store the previous one in the Recent files list
		if (!lastPosition.filePath.isEmpty())
		{
			_recentFiles.updateWith(lastPosition);
			updateRecentFilesMenu();
		}

		_reader.goToWord(position);

		CSettings().setValue(UI_OPEN_FILE_LAST_USED_DIR_SETTING, filePath);
		setWindowTitle(qApp->applicationName() % " - " % QFileInfo(filePath).baseName());
		_chapterProgressBar->setVisible(true);
	}
	else
		_chapterProgressBar->setVisible(false);
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
		_bookmarks.emplace_back(entry);

	updateBookmarksMenuItemsList();
}

void CMainWindow::saveBookmarksToSettings() const
{
	QStringList serialzedBookmarks;
	for (const CBookmark& bm : _bookmarks)
		serialzedBookmarks.push_back(bm.toString());

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
			openBookmark(bm);
		});
	}
}

void CMainWindow::updateRecentFilesMenu()
{
	QMenu * menu = ui->menuOpen_recent;
	menu->clear();

	for (const CBookmark& item : _recentFiles.items())
	{
		menu->addAction(item.filePath, [item, this](){
			_reader.loadFromFile(item.filePath);
			_reader.goToWord(item.wordIndex);
		});
	}
}

void CMainWindow::toggleFullScreen()
{
	setFullScreen(!isFullScreen());
}

void CMainWindow::setFullScreen(bool fullScreen)
{
	if (fullScreen)
	{
		showFullScreen();
		menuBar()->hide();
		if (CSettings().value(UI_STATUSBAR_HIDE_IN_FULLSCREEN_SETTING, UI_STATUSBAR_HIDE_IN_FULLSCREEN_DEFAULT).toBool())
			statusBar()->hide();
	}
	else
	{
		showNormal();
		menuBar()->show();
		statusBar()->show();
	}
}

void CMainWindow::settingsChanged()
{
	CSettings s;
	_reader.setLongWordPauseScaling(s.value(UI_LONG_WORD_THRESHOLD_SETTING, UI_LONG_WORD_THRESHOLD_DEFAULT).toUInt(),
									s.value(UI_LONG_WORD_DELAY_FACTOR_SETTING, UI_LONG_WORD_DELAY_FACTOR_DEFAULT).toFloat());
}

void CMainWindow::updateDisplay(const size_t currentTextFragmentIndex)
{
	const auto& currentFragment = _reader.textFragment(currentTextFragmentIndex);
	ui->_text->setText(currentFragment._textFragment, ui->actionShow_pivot->isChecked(), (TextFragment::PivotCalculationMethod)CSettings().value(UI_PIVOT_CALCULATION_METHOD, UI_PIVOT_CALCULATION_METHOD_DEFAULT).toInt());

	const auto chapterProgress = _reader.currentChapterProgress();
	_chapterProgressBar->setValue(chapterProgress.progressPercentage());
	_chapterProgressBar->setToolTip(tr("%1 out of %2 words read in this chapter\n%3 remaining")
		.arg(chapterProgress.wordsRead)
		.arg(chapterProgress.totalNumWords)
		.arg(secondsToHhhMmSs(_reader.currentChapterTimeRemainingSeconds()))
		);
}

void CMainWindow::updateInfo()
{
	_progressLabel->setText(
		tr("Reading word %1 out of %2 total (%3%); estimated time remaining: %4")
		.arg(_reader.totalNumWords() > 0 ? _reader.position() + 1 : 0)
		.arg(_reader.totalNumWords())
		.arg(QString::number(100 * (double)_reader.progress(), 'f', 2))
		.arg(secondsToHhhMmSs(_reader.timeRemainingSeconds()))
	);
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
