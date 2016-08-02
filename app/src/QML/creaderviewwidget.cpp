#include "creaderviewwidget.h"
#include "creaderview.h"

CReaderViewWidget::CReaderViewWidget(QWidget* parent) : QQuickWidget(parent)
{
	setSource(QUrl("qrc:/QML/creaderview.qml"));
}

CReaderView* CReaderViewWidget::readerView() const
{
	return dynamic_cast<CReaderView*>(rootObject());
}

const QFont& CReaderViewWidget::readerFont() const
{
	return readerView()->font();
}

void CReaderViewWidget::setReaderFont(const QFont& font)
{
	readerView()->setFont(font);
}
