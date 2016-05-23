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
	_textFadeEffect.setOpacity(1.0f);
	_textFadeOutAnimation = new QPropertyAnimation(&_textFadeEffect, "opacity", this);
	_textFadeOutAnimation->setStartValue(1.0f);
	_textFadeOutAnimation->setEndValue(0.0f);
	_textFadeOutAnimation->setEasingCurve(QEasingCurve::OutQuad);
	_textFadeOutAnimation->setDuration(100);

	setGraphicsEffect(&_textFadeEffect);
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

		_textFadeEffect.setOpacity(1.0f);

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

void CReaderView::paintEvent(QPaintEvent* e)
{
	QWidget::paintEvent(e);

	if (_text.isEmpty())
		return;

	QPainter p(this);
	QFontMetrics fontMetrics(font());
// 	p.drawLine(width() / 2, 0, width() / 2, height());
// 	p.drawLine(0, height() / 2, width(), height() / 2);

	const int centerCharIndex = _pivotCharacterIndex >= 0 ? _pivotCharacterIndex : _text.length() / 2;
	const QPoint textOffset(width() / 2 - fontMetrics.width(_text, centerCharIndex) - fontMetrics.width(_text[centerCharIndex]) / 2, height() / 2 - fontMetrics.height() / 2);

	QTextDocument doc;
	doc.setDefaultFont(font());
	if (_pivotCharacterIndex >= 0)
	{
		p.drawLine(width() / 2, height() / 2 - 3 * fontMetrics.height() / 2, width() / 2, height() / 2 - fontMetrics.height());
		p.drawLine(width() / 2, height() / 2 + fontMetrics.height(), width() / 2, height() / 2 + 3 * fontMetrics.height() / 2);

		doc.setHtml(_text.left(_pivotCharacterIndex) % "<font color=\"red\">" % _text[_pivotCharacterIndex] % "</font>" % _text.mid(_pivotCharacterIndex + 1));
	}
	else
		doc.setPlainText(_text);

	p.translate(textOffset);

	doc.drawContents(&p);
}
