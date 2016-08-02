#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QPixmap>
#include <QQuickPaintedItem>
RESTORE_COMPILER_WARNINGS

class QPropertyAnimation;

class CReaderView : public QQuickPaintedItem
{
	Q_OBJECT

	// Has to be a property in order for QPropertyAnimation to work with it
	Q_PROPERTY(qreal textOpacity WRITE setTextOpacity MEMBER _textOpacity)

public:
	explicit CReaderView(QQuickItem *parent = nullptr);
	~CReaderView();

public slots:
	QString text() const;
	void setText(const QString& newText, bool showPivot, int pivotCharacterIndex);
	void clear();

	void setTextOpacity(qreal opacity);

	void setBrightnessPercentage(int brightness);

	const QFont& font() const;
	void setFont(const QFont& font);
	void setFontSizePoints(int pointSize);

protected:
	void paint(QPainter *painter) override;

private:
// Data
	QString _text;
	int _pivotCharacterIndex = -1;

// Style
	QFont _font;

// Rendering
	QPropertyAnimation* _textFadeOutAnimation = nullptr;

	QPixmap _backgroundPixmap;
	qreal _textOpacity = 1.0f; // For dynamic effects
	int _brightnessPercentage = 100;
};

