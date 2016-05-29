#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QGraphicsOpacityEffect>
#include <QWidget>
RESTORE_COMPILER_WARNINGS

class QPropertyAnimation;

class CReaderView : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor pivotCharacterColor MEMBER _pivotCharacterColor WRITE setPivotCharacterColor DESIGNABLE true);
	Q_PROPERTY(QColor textBackgroundColor MEMBER _textBackgroundColor WRITE setTextBackgroundColor DESIGNABLE true);

public:
	CReaderView(QWidget* parent = 0);
	~CReaderView();

	void setPivotCharacterColor(const QColor& color);
	void setTextBackgroundColor(const QColor& color);

	void setText(const QString& text, int pivotCharacterIndex = -1);
	QString text() const;
	void clear();

protected:
	void paintEvent(QPaintEvent* e) override;

private:
	QColor _textBackgroundColor;
	QColor _pivotCharacterColor;

	QString _text;
	int _pivotCharacterIndex = -1;

	QPropertyAnimation* _textFadeOutAnimation = nullptr;
	QGraphicsOpacityEffect _textFadeEffect;
};

