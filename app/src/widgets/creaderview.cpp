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
	_textFadeOutAnimation->setDuration(75);
}

CReaderView::~CReaderView()
{
	_textFadeOutAnimation->stop();
}

QString CReaderView::text() const
{
	return _text.text();
}

void CReaderView::setText(const TextFragment& textFragment, bool showPivot, TextFragment::PivotCalculationMethod pivotCalculationMethod)
{
	const auto setTextImplementation = [textFragment, showPivot, pivotCalculationMethod, this]() {
		_text = textFragment;
		_pivotCharacterIndex = showPivot ? textFragment.pivotLetterIndex(pivotCalculationMethod) : -1;
		_textOpacity = 1.0f;

		update();
	};

	if (_text.text() == textFragment.text() || textFragment.isEmpty())
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
	_text = TextFragment();
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

	QFontMetrics fontMetrics(font());
	backgroundPainter.fillRect(0, height() / 2 - 3 * fontMetrics.height() / 2, width(), 3 * fontMetrics.height(), _textBackgroundColor);

	const QString string = _text.text();
	QTextDocument doc;

	if (!string.isEmpty())
	{
		doc.setDefaultFont(font());

		const QColor& textColor = palette().color(QPalette::Text);

		if (_pivotCharacterIndex >= 0)
		{
			backgroundPainter.setPen(textColor);
			backgroundPainter.drawLine(width() / 2, height() / 2 - 3 * fontMetrics.height() / 2, width() / 2, height() / 2 - 8 * fontMetrics.height() / 10);
			backgroundPainter.drawLine(width() / 2, height() / 2 + 8 * fontMetrics.height() / 10, width() / 2, height() / 2 + 3 * fontMetrics.height() / 2);

			doc.setHtml(
				coloredHtmlText(string.left(_pivotCharacterIndex), textColor)
				% coloredHtmlText(QString(string[_pivotCharacterIndex]), _pivotCharacterColor.name())
				% coloredHtmlText(string.mid(_pivotCharacterIndex + 1), textColor)
			);
		}
		else
			doc.setHtml(coloredHtmlText(string, textColor));
	}

	QPainter mainPainter(this);
	mainPainter.drawPixmap(0, 0, _backgroundPixmap);
	if (!string.isEmpty())
	{
		const int centerCharIndex = _pivotCharacterIndex >= 0 ? _pivotCharacterIndex : string.length() / 2;
		const QPoint textOffset(width() / 2 - fontMetrics.width(string, centerCharIndex) - fontMetrics.width(string[centerCharIndex]) / 2, height() / 2 - fontMetrics.height() / 2);
		mainPainter.translate(textOffset);
	}
	mainPainter.setOpacity(_textOpacity);
	doc.drawContents(&mainPainter);
}
