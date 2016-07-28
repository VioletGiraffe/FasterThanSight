#include "cthemeprovider.h"

#include "settings/csettings.h"
#include "assert/advanced_assert.h"
#include "../uisettings.h"

DISABLE_COMPILER_WARNINGS
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

CThemeProvider& CThemeProvider::instance()
{
	static CThemeProvider inst;
	return inst;
}

CThemeProvider::CThemeProvider()
{
	const auto themes = acceptedThemes();
	_themes = themes.first;
	_currentThemeIndex = themes.second;
}

size_t CThemeProvider::currentThemeIndex() const
{
	return _currentThemeIndex;
}

void CThemeProvider::setCurrentThemeIndex(size_t index)
{
	assert_and_return_r(_currentThemeIndex < _themes.size(), );
	_currentThemeIndex = index;
}

size_t CThemeProvider::acceptedThemeIndex() const
{
	const auto themes = acceptedThemes();
	assert_and_return_r(themes.second < themes.first.size(), 0);

	return themes.second;
}

CThemeProvider::Theme& CThemeProvider::currentTheme()
{
	if (_currentThemeIndex >= _themes.size())
	{
		static Theme dummy(QString::null);
		return dummy;
	}

	return _themes[_currentThemeIndex];
}

const CThemeProvider::Theme& CThemeProvider::currentTheme() const
{
	if (_currentThemeIndex >= _themes.size())
	{
		static const Theme dummy(QString::null);
		return dummy;
	}

	return _themes[_currentThemeIndex];
}

const CThemeProvider::Theme& CThemeProvider::acceptedTheme() const
{
	static const Theme dummy(QString::null);

	const auto themes = acceptedThemes();
	assert_and_return_r(themes.second < themes.first.size(), dummy);

	return themes.first[themes.second];
}

const std::deque<CThemeProvider::Theme>& CThemeProvider::currentThemes() const
{
	return _themes;
}

std::pair<std::deque<CThemeProvider::Theme>, size_t> CThemeProvider::acceptedThemes()
{
	std::deque<Theme> themesContainer;

	const QStringList themes = CSettings().value(THEMES_SETTING, DEFAULT_THEMES_LIST).toStringList();
	for (const QString& theme : themes)
		themesContainer.emplace_back(theme);

	const size_t currentThemeIndex = (size_t)CSettings().value(CURRENT_THEME_SETTING, 12).toULongLong();
	return std::make_pair(themesContainer, currentThemeIndex);
}

void CThemeProvider::addNewTheme(const Theme& theme)
{
	_themes.push_back(theme);
}

void CThemeProvider::deleteCurrentTheme()
{
	assert_and_return_r(_currentThemeIndex < _themes.size(), );
	_themes.erase(_themes.begin() + _currentThemeIndex);
}

void CThemeProvider::saveThemes() const
{
	QStringList list;
	for (const Theme& theme : _themes)
		list.push_back(theme.toString() + ';');

	CSettings().setValue(THEMES_SETTING, list);
	CSettings().setValue(CURRENT_THEME_SETTING, (qulonglong)_currentThemeIndex);
}

CThemeProvider::Theme::Theme(const QString& str)
{
	const auto components = str.split(';', QString::SkipEmptyParts);
	assert_and_return_r(components.size() == 5, );

	_name = components[0];
	_windowBgColor = QVariant(components[1]).value<QColor>();
	_textBgColor = QVariant(components[2]).value<QColor>();
	_textColor = QVariant(components[3]).value<QColor>();
	_pivotColor = QVariant(components[4]).value<QColor>();
}

QString CThemeProvider::Theme::toString() const
{
	return
		_name % ';' %
		QVariant(_windowBgColor).toString() % ';' %
		QVariant(_textBgColor).toString() % ';' %
		QVariant(_textColor).toString() % ';' %
		QVariant(_pivotColor).toString()
		;
}

// TODO: brightness
//const int brightnessPercentage = CSettings().value(UI_BRIGHTNESS, UI_BRIGHTNESS_DEFAULT).toInt();

inline QColor adjustBrightness(const QColor& c, const int brightnessPercentage)
{
	return QColor(c.red() * brightnessPercentage / 100, c.green() * brightnessPercentage / 100, c.blue() * brightnessPercentage / 100);
}