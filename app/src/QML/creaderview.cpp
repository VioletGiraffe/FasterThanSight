#include "creaderview.h"
#include "assert/advanced_assert.h"
#include "../styling/cthemeprovider.h"

DISABLE_COMPILER_WARNINGS
#include <QFontMetrics>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStringBuilder>
#include <QTextDocument>
RESTORE_COMPILER_WARNINGS

CReaderView::CReaderView(QQuickItem* parent) : QQuickPaintedItem(parent)
{
	setOpaquePainting(true);

	_textFadeOutAnimation = new QPropertyAnimation(this, "textOpacity", this);
	_textFadeOutAnimation->setStartValue(1.0f);
	_textFadeOutAnimation->setEndValue(0.0f);
	_textFadeOutAnimation->setEasingCurve(QEasingCurve::OutQuad);
	_textFadeOutAnimation->setDuration(75);

	connect(&CThemeProvider::instance(), &CThemeProvider::currentThemeChanged, [this](){
		update();
	});
}

CReaderView::~CReaderView()
{
	_textFadeOutAnimation->stop();
}

QString CReaderView::text() const
{
	return _text;
}

void CReaderView::setText(const QString& newText, bool showPivot, int pivotCharacterIndex)
{
	assert(pivotCharacterIndex >= 0 || !showPivot);

	const auto setTextImplementation = [this, newText, showPivot, pivotCharacterIndex]() {
		_text = newText;
		_pivotCharacterIndex = showPivot ? pivotCharacterIndex : -1;
		_textOpacity = 1.0f;

		update();
	};

	if (_text == newText || newText.isEmpty())
	{
		QMetaObject::Connection* connection = new QMetaObject::Connection();
		*connection = connect(_textFadeOutAnimation, &QPropertyAnimation::finished, [this, connection, setTextImplementation]() {
			disconnect(*connection);
			delete connection;

			setTextImplementation();
		});

		_textFadeOutAnimation->start();
	}
	else
		setTextImplementation();

}

void CReaderView::clear()
{
	_text.clear();
	_pivotCharacterIndex = -1;

	update();
}

void CReaderView::setTextOpacity(qreal opacity)
{
	_textOpacity = opacity;
	update();
}

const QFont& CReaderView::font() const
{
	return _font;
}

void CReaderView::setFont(const QFont& font)
{
	_font = font;
	update();
}

inline QString coloredHtmlText(const QString& text, const QColor& color)
{
	return "<font color=\"" % color.name() % "\">" % text % "</font>";
}

void CReaderView::paint(QPainter* painter)
{
	assert_and_return_r(painter, );

	CThemeProvider& themeProvider = CThemeProvider::instance();

	_backgroundPixmap = QPixmap((int)width(), (int)height());
	QPainter backgroundPainter(&_backgroundPixmap);
	backgroundPainter.fillRect(0, 0, (int)width(), (int)height(), themeProvider.currentTheme()._windowBgColor);

	QFontMetrics fontMetrics(_font);
	backgroundPainter.fillRect(0, (int)height() / 2 - 3 * fontMetrics.height() / 2, (int)width(), 3 * fontMetrics.height(), themeProvider.currentTheme()._textBgColor);

	QTextDocument doc;

	if (!_text.isEmpty())
	{
		doc.setDefaultFont(_font);

		const QColor& textColor = themeProvider.currentTheme()._textColor;

		if (_pivotCharacterIndex >= 0)
		{
			backgroundPainter.setPen(textColor);
			backgroundPainter.drawLine((int)width() / 2, (int)height() / 2 - 3 * fontMetrics.height() / 2, (int)width() / 2, (int)height() / 2 - 8 * fontMetrics.height() / 10);
			backgroundPainter.drawLine((int)width() / 2, (int)height() / 2 + 8 * fontMetrics.height() / 10, (int)width() / 2, (int)height() / 2 + 3 * fontMetrics.height() / 2);

			doc.setHtml(
				coloredHtmlText(_text.left(_pivotCharacterIndex), textColor)
				% coloredHtmlText(QString(_text[_pivotCharacterIndex]), themeProvider.currentTheme()._pivotColor.name())
				% coloredHtmlText(_text.mid(_pivotCharacterIndex + 1), textColor)
			);
		}
		else
			doc.setHtml(coloredHtmlText(_text, textColor));
	}

	painter->drawPixmap(0, 0, _backgroundPixmap);
	if (!_text.isEmpty())
	{
		const int centerCharIndex = _pivotCharacterIndex >= 0 ? _pivotCharacterIndex : _text.length() / 2;
		const QPoint textOffset((int)width() / 2 - fontMetrics.width(_text, centerCharIndex) - fontMetrics.width(_text[centerCharIndex]) / 2, (int)height() / 2 - fontMetrics.height() / 2);
		painter->translate(textOffset);
	}
	painter->setOpacity(_textOpacity);
	doc.drawContents(painter);
}
