#include "ccolorsdialog.h"
#include "ui_ccolorsdialog.h"

#include "settings/csettings.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QColorDialog>
#include <QDebug>
RESTORE_COMPILER_WARNINGS

#define WINDOW_BG_COLOR QStringLiteral("Color/WindowBgColor")
#define TEXT_BG_COLOR QStringLiteral("Color/TextBgColor")
#define TEXT_COLOR QStringLiteral("Color/TextColor")
#define PIVOT_COLOR QStringLiteral("Color/PivotColor")

CColorsDialog::CColorsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CColorsDialog)
{
	ui->setupUi(this);

	CSettings s;
	_windowBgColor = s.value(WINDOW_BG_COLOR, qApp->palette().background().color()).value<QColor>();
	_textBgColor = s.value(TEXT_BG_COLOR, qApp->palette().background().color()).value<QColor>();
	_textColor = s.value(TEXT_COLOR, qApp->palette().text().color()).value<QColor>();
	_pivotColor = s.value(PIVOT_COLOR, QColor::fromRgb(255, 36, 0)).value<QColor>();

	initColorPicker(ui->btnWindowBackgroundColor, _windowBgColor);
	initColorPicker(ui->btnTextBackgroundColor, _textBgColor);
	initColorPicker(ui->btnTextColor, _textColor);
	initColorPicker(ui->btnPivotColor, _pivotColor);
}

CColorsDialog::~CColorsDialog()
{
	delete ui;
}

void CColorsDialog::accept()
{
	CSettings s;
	s.setValue(WINDOW_BG_COLOR, _windowBgColor);
	s.setValue(TEXT_BG_COLOR, _textBgColor);
	s.setValue(TEXT_COLOR, _textColor);
	s.setValue(PIVOT_COLOR, _pivotColor);

	QDialog::accept();
}

void CColorsDialog::reject()
{
	qApp->setStyleSheet(storedStyle());
	QDialog::reject();
}

static const QString styleTemplate =
"CReaderView {"
"background-color: %1;"
"qproperty-textBackgroundColor: %2;"
"color: %3;"
"qproperty-pivotCharacterColor: %4;"
"}";

QString CColorsDialog::storedStyle()
{
	CSettings s;
	return styleTemplate
		.arg(s.value(WINDOW_BG_COLOR, qApp->palette().background().color()).value<QColor>().name())
		.arg(s.value(TEXT_BG_COLOR, qApp->palette().background().color()).value<QColor>().name())
		.arg(s.value(TEXT_COLOR, qApp->palette().text().color()).value<QColor>().name())
		.arg(s.value(PIVOT_COLOR, QColor::fromRgb(255, 36, 0)).value<QColor>().name());
}

QString CColorsDialog::temporaryStyle() const
{
	return styleTemplate
		.arg(_windowBgColor.name())
		.arg(_textBgColor.name())
		.arg(_textColor.name())
		.arg(_pivotColor.name());
}

void CColorsDialog::initColorPicker(QToolButton* btn, QColor& color)
{
	btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));

	connect(btn, &QToolButton::clicked, [this, &color, btn](){
		const QColor c = QColorDialog::getColor(color, this, "Pick a color");
		if (!c.isValid())
			return;

		color = c;
		btn->setStyleSheet(QString("background-color: %1;").arg(color.name()));
		qApp->setStyleSheet(temporaryStyle());
	});
}
