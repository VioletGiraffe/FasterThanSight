#include "cthemesdialog.h"

#include "settings/csettings.h"
#include "assert/advanced_assert.h"
#include "../uisettings.h"

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
static const QStringList DEFAULT_THEMES_LIST {
	"1337;#191919;#272727;#fffbe6;#ff5e5e;",
	"Blue Dawn;#0e1217;#10151c;#effbff;#e9ee00;",
	"Bliss;#191919;#282a2e;#ced1cf;#abfdd8;",
	"Dark Room (Contrast);#000000;#000000;#cda869;#399364;",
	"Dark Krystal;#000000;#0c0c13;#e7ffd2;#06b67e;",
	"FireCode;#191919;#191919;#cbcbda;#c93234;",
	"GitHub v2;#eee9cb;#eee9cb;#2b2b2b;#a71d5d;",
	"Green;#141610;#1b1d16;#a9ff47;#e6ffa4;",
	"Grunge;#21231d;#2d2f28;#e4f8f2;#a8f27f;",
	"Hail;#0d1115;#14191f;#aec2e0;#99fefe;",
	"Lavender*;#120f15;#1f1a24;#e0ceed;#989dfa;",
	"Legacy Pack;#040708;#080c0d;#dae8e1;#e8370d;",
	"NaturalContrast;#0f120c;#1b2016;#ffffff;#ff635f;",
	"Patriot;#1f2224;#2d3133;#cad9d0;#3790de;"
};

CThemesDialog::CThemesDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	connect(ui->_cbTheme, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, [this](int index){
		if (index < 0)
			return;

		_currentThemeIndex = (size_t)index;
		assert_and_return_r(_currentThemeIndex < _themes.size(), );
		Theme& theme = currentTheme();
		initColorPicker(ui->btnWindowBackgroundColor, theme._windowBgColor);
		initColorPicker(ui->btnTextBackgroundColor, theme._textBgColor);
		initColorPicker(ui->btnTextColor, theme._textColor);
		initColorPicker(ui->btnPivotColor, theme._pivotColor);

		qApp->setStyleSheet(theme.style());
	});

	loadThemes();

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

	const size_t currentThemeIndex = (size_t)CSettings().value(CURRENT_THEME_SETTING, 12).toULongLong();
	return std::make_pair(themesContainer, currentThemeIndex);
}

void CThemesDialog::saveThemes() const
{
	QStringList list;
	for (const Theme& theme :_themes)
		list.push_back(theme.toString() + ';');

	CSettings().setValue(THEMES_SETTING, list);
	CSettings().setValue(CURRENT_THEME_SETTING, (qulonglong)_currentThemeIndex);
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

	Theme newTheme = currentTheme();
	newTheme._name = themeName;
	_themes.push_back(newTheme);
	ui->_cbTheme->addItem(themeName);
	ui->_cbTheme->setCurrentIndex(ui->_cbTheme->count() - 1);
}

void CThemesDialog::deleteTheme()
{
	assert_and_return_r(_currentThemeIndex < _themes.size(), );
	_themes.erase(_themes.begin() + _currentThemeIndex);
	ui->_cbTheme->removeItem(_currentThemeIndex);
}

void CThemesDialog::loadThemes()
{
	ui->_cbTheme->blockSignals(true);
	ui->_cbTheme->clear();

	const auto themes = themesFromSettings();
	_themes = themes.first;
	_currentThemeIndex = themes.second;

	for (const auto& theme : _themes)
		ui->_cbTheme->addItem(theme._name);

	ui->_cbTheme->blockSignals(false);

	ui->_cbTheme->setCurrentIndex(_currentThemeIndex);
	ui->_cbTheme->currentIndexChanged(_currentThemeIndex);
}

void CThemesDialog::showEvent(QShowEvent* e)
{
	loadThemes();
	QDialog::showEvent(e);
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

inline QColor adjustBrightness(const QColor& c, const int brightnessPercentage)
{
	return QColor(c.red() * brightnessPercentage / 100, c.green() * brightnessPercentage / 100, c.blue() * brightnessPercentage / 100);
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

	const int brightnessPercentage = CSettings().value(UI_BRIGHTNESS, UI_BRIGHTNESS_DEFAULT).toInt();

	return styleTemplate
		.arg(adjustBrightness(_windowBgColor, brightnessPercentage).name())
		.arg(adjustBrightness(_textBgColor, brightnessPercentage).name())
		.arg(adjustBrightness(_textColor, brightnessPercentage).name())
		.arg(adjustBrightness(_pivotColor, brightnessPercentage).name());
}
