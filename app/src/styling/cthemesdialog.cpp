#include "cthemesdialog.h"

#include "settings/csettings.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include "ui_cthemesdialog.h"

#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

#define THEMES_SETTING QStringLiteral("Themes/ThemesList")
#define CURRENT_THEME_SETTING QStringLiteral("Themes/CurrentTheme")
#define DEFAULT_THEMES_LIST QStringList {"1337;#191919;#272727;#fffbe6;#ff5e5e;", "Blue Dawn;#0f1318;#0f1318;#effbff;#e9ee00;", "Bliss;#191919;#282A2E;#CED1CF;#abfdd8;"}

CThemesDialog::CThemesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	const auto themes = themesFromSettings();
	_themes = themes.first;
	_currentThemeIndex = themes.second;
	for (const auto& theme: _themes)
		ui->_cbTheme->addItem(theme._name);

	connect(ui->_cbTheme, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, [this](int index){
		_currentThemeIndex = (size_t)index;
		assert_and_return_r(_currentThemeIndex < _themes.size(), );
		Theme& theme = currentTheme();
		initColorPicker(ui->btnWindowBackgroundColor, theme._windowBgColor);
		initColorPicker(ui->btnTextBackgroundColor, theme._textBgColor);
		initColorPicker(ui->btnTextColor, theme._textColor);
		initColorPicker(ui->btnPivotColor, theme._pivotColor);

		qApp->setStyleSheet(theme.style());
	});

	ui->_cbTheme->setCurrentIndex(_currentThemeIndex);
	ui->_cbTheme->currentIndexChanged(_currentThemeIndex);

	connect(ui->_btnCopy, &QPushButton::clicked, [this](){
		createNewTheme();
	});

	connect(ui->_btnDelete, &QPushButton::clicked, [this]() {
		deleteTheme();
	});

	connect(this, &QDialog::rejected, [this]() {
		qApp->setStyleSheet(currentAcceptedStyle());
	});

	connect(this, &QDialog::accepted, [this]() {
		saveThemes();
	});
}

CThemesDialog::~CThemesDialog()
{
	delete ui;
}

QString CThemesDialog::currentAcceptedStyle()
{
	const auto themes = themesFromSettings();
	assert_and_return_r(themes.second < themes.first.size(), QString());

	return themes.first[themes.second].style();
}

QString CThemesDialog::currentStyle() const
{
	assert_and_return_r(_currentThemeIndex < _themes.size(), QString());

	const Theme& theme = _themes[_currentThemeIndex];
	return theme.style();
}

CThemesDialog::Theme& CThemesDialog::currentTheme()
{
	if (_currentThemeIndex >= _themes.size())
	{
		static Theme dummy(QString::null);
		return dummy;
	}

	return _themes[_currentThemeIndex];
}

const CThemesDialog::Theme& CThemesDialog::currentTheme() const
{
	if (_currentThemeIndex >= _themes.size())
	{
		static const Theme dummy(QString::null);
		return dummy;
	}

	return _themes[_currentThemeIndex];
}

std::pair<std::deque<CThemesDialog::Theme>, size_t> CThemesDialog::themesFromSettings()
{
	std::deque<Theme> themesContainer;

	const QStringList themes = CSettings().value(THEMES_SETTING, DEFAULT_THEMES_LIST).toStringList();
	for (const QString& theme: themes)
		themesContainer.emplace_back(theme);

	const size_t currentThemeIndex = (size_t)CSettings().value(CURRENT_THEME_SETTING, 0).toULongLong();
	return std::make_pair(themesContainer, currentThemeIndex);
}

void CThemesDialog::saveThemes() const
{
	QStringList list;
	for (const Theme& theme :_themes)
		list.push_back(theme.toString() + ';');

	CSettings().setValue(THEMES_SETTING, list);
	CSettings().setValue(CURRENT_THEME_SETTING, _currentThemeIndex);
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
		qApp->setStyleSheet(currentStyle());
	});
}

void CThemesDialog::createNewTheme()
{
	const QString themeName = QInputDialog::getText(this, "New theme", "Enter a name for the new theme:", QLineEdit::Normal, currentTheme()._name + " (Copy)");
	if (std::find_if(_themes.begin(), _themes.end(), [&themeName](const Theme& theme){return theme._name.toLower() == themeName.toLower();}) != _themes.end())
	{
		QMessageBox::information(this, "Name already exists", "A theme named \"" % themeName % "\" already exists, pick a different name.");
		return;
	}
}

void CThemesDialog::deleteTheme()
{

}

CThemesDialog::Theme::Theme(const QString& str)
{
	const auto components = str.split(';', QString::SkipEmptyParts);
	assert_and_return_r(components.size() == 5, );

	_name = components[0];
	_windowBgColor = QVariant(components[1]).value<QColor>();
	_textBgColor = QVariant(components[2]).value<QColor>();
	_textColor = QVariant(components[3]).value<QColor>();
	_pivotColor = QVariant(components[4]).value<QColor>();
}

QString CThemesDialog::Theme::toString() const
{
	return
		_name % ';' %
		QVariant(_windowBgColor).toString() % ';' %
		QVariant(_textBgColor).toString() % ';' %
		QVariant(_textColor).toString() % ';' %
		QVariant(_pivotColor).toString()
		;
}

QString CThemesDialog::Theme::style() const
{
	static const QString styleTemplate =
		"CReaderView {"
		"background-color: %1; "
		"qproperty-textBackgroundColor: %2; "
		"color: %3; "
		"qproperty-pivotCharacterColor: %4; "
		"}";

	return styleTemplate
		.arg(_windowBgColor.name())
		.arg(_textBgColor.name())
		.arg(_textColor.name())
		.arg(_pivotColor.name());
}
