#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QWidget>
RESTORE_COMPILER_WARNINGS

class CReaderView : public QWidget
{
public:
	CReaderView(QWidget* parent = 0);

	void setText(const QString& text, int pivotCharacterIndex = -1);
	QString text() const;
	void clear();

protected:
	void paintEvent(QPaintEvent* e) override;

private:
	QString _text;
	int _pivotCharacterIndex = -1;
};

