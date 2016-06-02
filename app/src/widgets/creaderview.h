#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QWidget>
RESTORE_COMPILER_WARNINGS

class QPropertyAnimation;

class CReaderView : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QColor pivotCharacterColor MEMBER _pivotCharacterColor DESIGNABLE true);
	Q_PROPERTY(QColor textBackgroundColor MEMBER _textBackgroundColor DESIGNABLE true);
	Q_PROPERTY(qreal textOpacity WRITE setTextOpacity MEMBER _textOpacity);

public:
	CReaderView(QWidget* parent = 0);
	~CReaderView();

	void setText(const QString& text, int pivotCharacterIndex = -1);
	QString text() const;
	void clear();

	void setTextOpacity(qreal opacity);

protected:
	void paintEvent(QPaintEvent* e) override;

private:
	QColor _textBackgroundColor;
	QColor _pivotCharacterColor;

	QString _text;
	int _pivotCharacterIndex = -1;

	QPropertyAnimation* _textFadeOutAnimation = nullptr;

	QPixmap _backgroundPixmap;
	qreal _textOpacity = 1.0f; // For dynamic effects 
};

