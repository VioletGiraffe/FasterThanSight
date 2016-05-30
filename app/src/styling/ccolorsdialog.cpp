#include "ccolorsdialog.h"

#include "settings/csettings.h"
#include "assert/advanced_assert.h"

DISABLE_COMPILER_WARNINGS
#include "ui_ccolorsdialog.h"

#include <QApplication>
#include <QColorDialog>
#include <QDebug>
#include <QStringBuilder>
RESTORE_COMPILER_WARNINGS

#define THEMES_SETTING QStringLiteral("Themes/ThemesList")
#define CURRENT_THEME_SETTING QStringLiteral("Themes/CurrentTheme")
#define DEFAULT_THEMES_LIST QStringList {"1337;#191919;#272727;#fffbe6;#ff5e5e;"}

CColorsDialog::CColorsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	const auto themes = themesFromSettings();
	_themes = themes.first;
	_currentThemeIndex = themes.second;
	for (const auto& theme: _themes)
		ui->_cbTheme->addItem(theme._name);

	ui->_cbTheme->setCurrentIndex(_currentThemeIndex);

	assert_and_return_r(_currentThemeIndex < _themes.size(), );
	Theme& theme = _themes[_currentThemeIndex];
	initColorPicker(ui->btnWindowBackgroundColor, theme._windowBgColor);
	initColorPicker(ui->btnTextBackgroundColor, theme._textBgColor);
	initColorPicker(ui->btnTextColor, theme._textColor);
	initColorPicker(ui->btnPivotColor, theme._pivotColor);
}

CColorsDialog::~CColorsDialog()
{
	delete ui;
}

QString CColorsDialog::currentAcceptedStyle()
{
	const auto themes = themesFromSettings();
	assert_and_return_r(themes.second < themes.first.size(), QString());

	return themes.first[themes.second].style();
}

QString CColorsDialog::currentStyle() const
{
	assert_and_return_r(_currentThemeIndex < _themes.size(), QString());

	const Theme& theme = _themes[_currentThemeIndex];
	return theme.style();
}

std::pair<std::deque<CColorsDialog::Theme>, size_t> CColorsDialog::themesFromSettings()
{
	std::deque<Theme> themesContainer;

	const QStringList themes = CSettings().value(THEMES_SETTING, DEFAULT_THEMES_LIST).toStringList();
	for (const QString& theme: themes)
		themesContainer.emplace_back(theme);

	const size_t currentThemeIndex = (size_t)CSettings().value(CURRENT_THEME_SETTING, 0).toULongLong();
	return std::make_pair(themesContainer, currentThemeIndex);
}

void CColorsDialog::saveThemes() const
{
	QStringList list;
	for (const Theme& theme :_themes)
		list.push_back(theme.toString() + ';');

	CSettings().setValue(THEMES_SETTING, list);
	CSettings().setValue(CURRENT_THEME_SETTING, _currentThemeIndex);
}

void CColorsDialog::initColorPicker(QToolButton* btn, QColor& color)
{
	btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));

	connect(btn, &QToolButton::clicked, [this, &color, btn](){
		const QColor c = QColorDialog::getColor(color, this, "Pick a color");
		if (!c.isValid())
			return;

		color = c;
		btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));
		qApp->setStyleSheet(currentStyle());
	});
}

CColorsDialog::Theme::Theme(const QString& str)
{
	const auto components = str.split(';', QString::SkipEmptyParts);
	assert_and_return_r(components.size() == 5, );

	_name = components[0];
	_windowBgColor = QVariant(components[1]).value<QColor>();
	_textBgColor = QVariant(components[2]).value<QColor>();
	_textColor = QVariant(components[3]).value<QColor>();
	_pivotColor = QVariant(components[4]).value<QColor>();
}

QString CColorsDialog::Theme::toString() const
{
	return
		_name % ';' %
		QVariant(_windowBgColor).toString() % ';' %
		QVariant(_textBgColor).toString() % ';' %
		QVariant(_textColor).toString() % ';' %
		QVariant(_pivotColor).toString()
		;
}

QString CColorsDialog::Theme::style() const
{
	static const QString styleTemplate =
		"CReaderView {"
		"background-color: %1;"
		"qproperty-textBackgroundColor: %2;"
		"color: %3;"
		"qproperty-pivotCharacterColor: %4;"
		"}";

	return styleTemplate
		.arg(_windowBgColor.name())
		.arg(_textBgColor.name())
		.arg(_textColor.name())
		.arg(_pivotColor.name());
}
