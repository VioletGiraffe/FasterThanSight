#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QQuickWidget>
RESTORE_COMPILER_WARNINGS

class CReaderView;

class CReaderViewWidget : public QQuickWidget
{
public:
	explicit CReaderViewWidget(QWidget *parent = nullptr);

	CReaderView* readerView() const;

	const QFont& readerFont() const;
	void setReaderFont(const QFont& font);
};
