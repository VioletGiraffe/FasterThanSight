#include "creaderview.h"

DISABLE_COMPILER_WARNINGS
#include <QFontMetrics>
#include <QPainter>
#include <QStringBuilder>
#include <QTextDocument>
RESTORE_COMPILER_WARNINGS

CReaderView::CReaderView(QWidget* parent) : QWidget(parent)
{
}

void CReaderView::setText(const QString& text, int pivotCharacterIndex /*= -1*/)
{
	_text = text;
	_pivotCharacterIndex = pivotCharacterIndex;

	update();
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

	p.drawLine(width() / 2, height() / 2 - 3 * fontMetrics.height() / 2, width() / 2, height() / 2 - fontMetrics.height());
	p.drawLine(width() / 2, height() / 2 + fontMetrics.height(), width() / 2, height() / 2 + 3 * fontMetrics.height() / 2);

	QTextDocument doc;
	doc.setDefaultFont(font());
	if (_pivotCharacterIndex >= 0)
		doc.setHtml(_text.left(_pivotCharacterIndex) % "<font color=\"red\">" % _text[_pivotCharacterIndex] % "</font>" % _text.mid(_pivotCharacterIndex + 1));
	else
		doc.setPlainText(_text);

	p.translate(textOffset);

	doc.drawContents(&p);
}
