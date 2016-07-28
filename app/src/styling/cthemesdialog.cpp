#include "cthemesdialog.h"
#include "cthemeprovider.h"

#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include "ui_cthemesdialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

CThemesDialog::CThemesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	connect(ui->_cbTheme, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, [this](int index){
		if (index < 0)
			return;

		CThemeProvider& themeProvider = CThemeProvider::instance();
		themeProvider.setCurrentThemeIndex(index);
		
		CThemeProvider::Theme& theme = themeProvider.currentTheme();
		initColorPicker(ui->btnWindowBackgroundColor, theme._windowBgColor);
		initColorPicker(ui->btnTextBackgroundColor, theme._textBgColor);
		initColorPicker(ui->btnTextColor, theme._textColor);
		initColorPicker(ui->btnPivotColor, theme._pivotColor);
	});

	connect(ui->_btnCopy, &QPushButton::clicked, this, &CThemesDialog::createNewTheme);
	connect(ui->_btnDelete, &QPushButton::clicked, this, &CThemesDialog::deleteTheme);

	connect(this, &QDialog::rejected, [this]() {
		CThemeProvider& themeProvider = CThemeProvider::instance();
		themeProvider.setCurrentThemeIndex(themeProvider.acceptedThemeIndex());
	});

	connect(this, &QDialog::accepted, [](){
		CThemeProvider::instance().saveThemes();
	});
}

CThemesDialog::~CThemesDialog()
{
	delete ui;
}

void CThemesDialog::loadThemes()
{
	ui->_cbTheme->blockSignals(true);
	ui->_cbTheme->clear();

	const auto themes = CThemeProvider::instance().acceptedThemes();

	for (const auto& theme : themes.first)
		ui->_cbTheme->addItem(theme._name);

	ui->_cbTheme->blockSignals(false);

	ui->_cbTheme->setCurrentIndex(themes.second);
	ui->_cbTheme->currentIndexChanged(themes.second);
}

void CThemesDialog::initColorPicker(QToolButton* btn, QColor& color)
{
	btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));

	btn->disconnect();

	connect(btn, &QToolButton::clicked, [this, &color, btn](){
		const QColor c = QColorDialog::getColor(color, this, "Pick a color");
		if (!c.isValid())
			return;

		color = c;
		btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));
		CThemeProvider& provider = CThemeProvider::instance();
		provider.notifyThemeChanged();
	});
}

void CThemesDialog::createNewTheme()
{
	CThemeProvider& themeProvider = CThemeProvider::instance();

	const QString themeName = QInputDialog::getText(this, "New theme", "Enter a name for the new theme:", QLineEdit::Normal, themeProvider.currentTheme()._name + " (Copy)");
	if (std::find_if(themeProvider.currentThemes().begin(), themeProvider.currentThemes().end(), [&themeName](const CThemeProvider::Theme& theme){
			return theme._name.toLower() == themeName.toLower();
		}) != themeProvider.currentThemes().end())
	{
		QMessageBox::information(this, "Name already exists", "A theme named \"" % themeName % "\" already exists, pick a different name.");
		return;
	}

	CThemeProvider::Theme newTheme = themeProvider.currentTheme();
	newTheme._name = themeName;
	themeProvider.addNewTheme(newTheme);
	ui->_cbTheme->addItem(themeName);
	ui->_cbTheme->setCurrentIndex(ui->_cbTheme->count() - 1);
}

void CThemesDialog::deleteTheme()
{
	CThemeProvider& themeProvider = CThemeProvider::instance();
	const auto currentThemeIndex = themeProvider.currentThemeIndex();
	themeProvider.deleteCurrentTheme();
	ui->_cbTheme->removeItem(currentThemeIndex);
}

void CThemesDialog::showEvent(QShowEvent* e)
{
	loadThemes();
	QDialog::showEvent(e);
}
