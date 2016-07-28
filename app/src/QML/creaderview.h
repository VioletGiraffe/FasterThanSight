#pragma once

#include "compiler/compiler_warnings_control.h"
#include "text/ctextfragment.h"

DISABLE_COMPILER_WARNINGS
#include <QPixmap>
#include <QQuickPaintedItem>
RESTORE_COMPILER_WARNINGS

class QPropertyAnimation;

class CReaderView : public QQuickPaintedItem
{
	Q_OBJECT

	Q_PROPERTY(qreal textOpacity WRITE setTextOpacity MEMBER _textOpacity)

public:
	explicit CReaderView(QQuickItem *parent = nullptr);
	~CReaderView();

	QString text() const;
	void setText(const TextFragment& textFragment, bool showPivot, TextFragment::PivotCalculationMethod pivotCalculationMethod = TextFragment::pcmMagic);
	void clear();

	void setTextOpacity(qreal opacity);

	const QFont& font() const;
	void setFont(const QFont& font);

protected:
	void paint(QPainter *painter) override;

private:
// Data
	TextFragment _text;
	int _pivotCharacterIndex = -1;

// Style
	QFont _font;

// Rendering
	QPropertyAnimation* _textFadeOutAnimation = nullptr;

	QPixmap _backgroundPixmap;
	qreal _textOpacity = 1.0f; // For dynamic effects
};

