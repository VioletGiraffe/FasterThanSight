#include "csettingspagepivot.h"

#include "textfragment/ctextfragment.h"
#include "settings/csettings.h"
#include "settings.h"
#include "../uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspagepivot.h"

#include <QDebug>
RESTORE_COMPILER_WARNINGS

CSettingsPagePivot::CSettingsPagePivot(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPagePivot)
{
	ui->setupUi(this);

	QFont textFont = ui->_demo->font();
	textFont.setFamily(CSettings().value(UI_FONT_FAMILY, textFont.family()).toString());
	textFont.setStyle((QFont::Style)CSettings().value(UI_FONT_STYLE, textFont.style()).toInt());
	textFont.setWeight((QFont::Weight)CSettings().value(UI_FONT_WEIGHT, textFont.weight()).toInt());
	textFont.setPointSize(CSettings().value(UI_FONT_SIZE_SETTING, textFont.pointSize()).toInt());
	ui->_demo->setFont(textFont);

	ui->_cbPivotCalculationMethod->addItem("Magic");
	ui->_cbPivotCalculationMethod->addItem("Middle");
	ui->_cbPivotCalculationMethod->addItem("Quarter");
	ui->_cbPivotCalculationMethod->addItem("Square root");
	ui->_cbPivotCalculationMethod->addItem("Cubic root");
	ui->_cbPivotCalculationMethod->addItem("Logarithm");

	connect(ui->_cbPivotCalculationMethod, (void (QComboBox::*)(int))&QComboBox::currentIndexChanged, [this](int index){
		const TextFragment::PivotCalculationMethod method = (TextFragment::PivotCalculationMethod)index;
		TextFragment fragment("Paradise", QString());
		ui->_demo->setText(fragment.word(), fragment.pivotLetterIndex(method));
	});

	ui->_cbPivotCalculationMethod->setCurrentIndex(CSettings().value(PIVOT_CALCULATION_METHOD, TextFragment::pcmMagic).toInt());
	ui->_cbPivotCalculationMethod->currentIndexChanged(ui->_cbPivotCalculationMethod->currentIndex());
}

CSettingsPagePivot::~CSettingsPagePivot()
{
	delete ui;
}

void CSettingsPagePivot::acceptSettings()
{
	CSettings s;
	s.setValue(PIVOT_CALCULATION_METHOD, ui->_cbPivotCalculationMethod->currentIndex());
}
