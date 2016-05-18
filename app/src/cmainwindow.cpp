#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QSlider>
#include <QToolBar>

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow),
	_reader(this)
{
	ui->setupUi(this);

	initToolBars();
	initActions();
}

CMainWindow::~CMainWindow()
{
	delete ui;
}

void CMainWindow::initToolBars()
{
	// Reading settings toolbar
	_textSizeSlider = new QSlider(Qt::Horizontal);
	_textSizeSlider->setMinimum(20);
	_textSizeSlider->setMaximum(300);
	connect(_textSizeSlider, &QSlider::valueChanged, [this](int size){
		QFont font = ui->_text->font();
		font.setPointSize(size);
		ui->_text->setFont(font);
	});
	_textSizeSlider->valueChanged(_textSizeSlider->value());

	_readingSettingsToolbar = addToolBar(tr("Reading settings"));
	_readingSettingsToolbar->addWidget(new QLabel(tr("Text size") + "  "));
	_readingSettingsToolbar->addWidget(_textSizeSlider);
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

	connect(ui->actionOpen, &QAction::triggered, [this](){
		const QString filePath = QFileDialog::getOpenFileName(this, "Pick a text file to open");
		if (!filePath.isEmpty())
			_reader.loadFromFile(filePath);
	});

	connect(ui->action_Start_resume, &QAction::triggered, [this](){
		_reader.resumeReading();
	});

	connect(ui->action_Pause, &QAction::triggered, [this](){
		_reader.pauseReading();
	});

	connect(ui->actionStop, &QAction::triggered, [this](){
		_reader.resetAndStop();
	});

	connect(ui->action_Exit, &QAction::triggered, qApp, &QApplication::exit);
}

void CMainWindow::displayText(const TextFragment& text)
{
	ui->_text->setText(text._text);
}

void CMainWindow::stateChanged(const CReader::State newState)
{

}
