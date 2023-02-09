#include "csettingspagepivot.h"

#include "text/ctextfragment.h"
#include "settings/csettings.h"
#include "../uisettings.h"

#include "QML/creaderview.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspagepivot.h"

#include <QDebug>
RESTORE_COMPILER_WARNINGS

#include <algorithm>

CSettingsPagePivot::CSettingsPagePivot(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPagePivot)
{
	ui->setupUi(this);

	QFont textFont = ui->_demo->font();
	textFont.setFamily(CSettings().value(UI_FONT_FAMILY, textFont.family()).toString());
	textFont.setStyle((QFont::Style)CSettings().value(UI_FONT_STYLE, textFont.style()).toInt());
	textFont.setWeight((QFont::Weight)CSettings().value(UI_FONT_WEIGHT, textFont.weight()).toInt());
	ui->_demo->setFont(textFont);

	ui->_cbPivotCalculationMethod->addItem("Magic");
	ui->_cbPivotCalculationMethod->addItem("Middle");
	ui->_cbPivotCalculationMethod->addItem("Quarter");
	ui->_cbPivotCalculationMethod->addItem("Square root");
	ui->_cbPivotCalculationMethod->addItem("Cubic root");
	ui->_cbPivotCalculationMethod->addItem("Logarithm");

	connect(ui->_cbPivotCalculationMethod, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, this, [this](int index){
		const TextFragment::PivotCalculationMethod method = (TextFragment::PivotCalculationMethod)index;
		CReaderView* readerView = ui->_demo->readerView();
		assert_and_return_r(readerView, );

		const QString testWord = QStringLiteral("Paradise");
		readerView->setText(testWord, true, TextFragment(testWord, QString()).pivotLetterIndex(method));
	});

	ui->_cbPivotCalculationMethod->setCurrentIndex(CSettings().value(UI_PIVOT_CALCULATION_METHOD, UI_PIVOT_CALCULATION_METHOD_DEFAULT).toInt());
	ui->_cbPivotCalculationMethod->currentIndexChanged(ui->_cbPivotCalculationMethod->currentIndex());
}

CSettingsPagePivot::~CSettingsPagePivot()
{
	delete ui;
}

void CSettingsPagePivot::acceptSettings()
{
	CSettings s;
	s.setValue(UI_PIVOT_CALCULATION_METHOD, ui->_cbPivotCalculationMethod->currentIndex());
}

void CSettingsPagePivot::resizeEvent(QResizeEvent* e)
{
	CSettingsPage::resizeEvent(e);

	QFont textFont = ui->_demo->font();
	CReaderView* readerView = ui->_demo->readerView();
	assert_and_return_r(readerView, );

	const float textScaleFactor = ui->_demo->width()/2.0f / std::max(QFontMetrics(textFont).horizontalAdvance(readerView->text()), 1);

	const int oldTextSize = textFont.pointSize();
	textFont.setPointSize((int)(textScaleFactor * oldTextSize));
	readerView->setFont(textFont);
}
