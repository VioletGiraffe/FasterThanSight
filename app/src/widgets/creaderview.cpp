#include "creaderview.h"

DISABLE_COMPILER_WARNINGS
#include <QFontMetrics>
#include <QPainter>
#include <QPropertyAnimation>
#include <QStringBuilder>
#include <QTextDocument>
RESTORE_COMPILER_WARNINGS

CReaderView::CReaderView(QWidget* parent) : QWidget(parent)
{
	_textFadeOutAnimation = new QPropertyAnimation(this, "textOpacity", this);
	_textFadeOutAnimation->setStartValue(1.0f);
	_textFadeOutAnimation->setEndValue(0.0f);
	_textFadeOutAnimation->setEasingCurve(QEasingCurve::OutQuad);
	_textFadeOutAnimation->setDuration(100);
}

CReaderView::~CReaderView()
{
	_textFadeOutAnimation->stop();
}

void CReaderView::setText(const QString& text, int pivotCharacterIndex /*= -1*/)
{
	const auto setTextImplementation = [text, pivotCharacterIndex, this]() {
		_text = text;
		_pivotCharacterIndex = pivotCharacterIndex;

		_textOpacity = 1.0f;

		update();
	};

	if (_text == text)
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

QString CReaderView::text() const
{
	return _text;
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

inline QString coloredHtmlText(const QString& text, const QColor& color)
{
	return "<font color=\"" % color.name() % "\">" % text % "</font>";
}

void CReaderView::paintEvent(QPaintEvent* /*e*/)
{
	_backgroundPixmap = QPixmap(size());
	QPainter backgroundPainter(&_backgroundPixmap);
	backgroundPainter.fillRect(rect(), palette().color(QPalette::Background));

	if (_text.isEmpty())
	{
		QPainter(this).drawPixmap(0, 0, _backgroundPixmap);
		return;
	}

	QFontMetrics fontMetrics(font());

	const int centerCharIndex = _pivotCharacterIndex >= 0 ? _pivotCharacterIndex : _text.length() / 2;
	const QPoint textOffset(width() / 2 - fontMetrics.width(_text, centerCharIndex) - fontMetrics.width(_text[centerCharIndex]) / 2, height() / 2 - fontMetrics.height() / 2);

	backgroundPainter.fillRect(0, height() / 2 - 3 * fontMetrics.height() / 2, width(), 3 * fontMetrics.height(), _textBackgroundColor);

	QTextDocument doc;
	doc.setDefaultFont(font());

	const QColor& textColor = palette().color(QPalette::Text);

	if (_pivotCharacterIndex >= 0)
	{
		backgroundPainter.setPen(textColor);
		backgroundPainter.drawLine(width() / 2, height() / 2 - 3 * fontMetrics.height() / 2, width() / 2, height() / 2 - 8 * fontMetrics.height() / 10);
		backgroundPainter.drawLine(width() / 2, height() / 2 + 8 * fontMetrics.height() / 10, width() / 2, height() / 2 + 3 * fontMetrics.height() / 2);

		doc.setHtml(
			coloredHtmlText(_text.left(_pivotCharacterIndex), textColor)
			% coloredHtmlText(QString(_text[_pivotCharacterIndex]), _pivotCharacterColor.name())
			% coloredHtmlText(_text.mid(_pivotCharacterIndex + 1), textColor)
		);
	}
	else
		doc.setHtml(coloredHtmlText(_text, textColor));

	QPainter mainPainter(this);
	mainPainter.drawPixmap(0, 0, _backgroundPixmap);
	mainPainter.translate(textOffset);
	mainPainter.setOpacity(_textOpacity);
	doc.drawContents(&mainPainter);
}
