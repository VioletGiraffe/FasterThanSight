#include "csettingspagepauses.h"

#include "settings/csettings.h"
#include "../uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspagepauses.h"
RESTORE_COMPILER_WARNINGS

CSettingsPagePauses::CSettingsPagePauses(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPagePauses)
{
	ui->setupUi(this);

	CSettings s;
	ui->_sbLongWordThreshold->setValue(s.value(UI_LONG_WORD_THRESHOLD_SETTING, UI_LONG_WORD_THRESHOLD_DEFAULT).toInt());
	ui->_sbLongWordDelayFactor->setValue(s.value(UI_LONG_WORD_DELAY_FACTOR_SETTING, UI_LONG_WORD_DELAY_FACTOR_DEFAULT).toDouble());
}

CSettingsPagePauses::~CSettingsPagePauses()
{
	delete ui;
}

void CSettingsPagePauses::acceptSettings()
{
	CSettings s;
	s.setValue(UI_LONG_WORD_THRESHOLD_SETTING, ui->_sbLongWordThreshold->value());
	s.setValue(UI_LONG_WORD_DELAY_FACTOR_SETTING, ui->_sbLongWordDelayFactor->value());
}
