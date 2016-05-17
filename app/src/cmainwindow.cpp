#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QSlider>
#include <QToolBar>

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow)
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
		const QString fileName = QFileDialog::getOpenFileName(this, "Pick a text file to open");
		if (!fileName.isEmpty())
			_txtParser.parseFile(fileName);
	});
}
