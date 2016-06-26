#include "csettingspageinterface.h"

#include "settings/csettings.h"
#include "../uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspageinterface.h"
RESTORE_COMPILER_WARNINGS

CSettingsPageInterface::CSettingsPageInterface(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPageInterface)
{
	ui->setupUi(this);

	CSettings s;

	ui->_cbHideStatusbrInFullscreen->setChecked(s.value(UI_STATUSBAR_HIDE_IN_FULLSCREEN_SETTING, UI_STATUSBAR_HIDE_IN_FULLSCREEN_DEFAULT).toBool());
}

CSettingsPageInterface::~CSettingsPageInterface()
{
	delete ui;
}

void CSettingsPageInterface::acceptSettings()
{
	CSettings s;
	s.setValue(UI_STATUSBAR_HIDE_IN_FULLSCREEN_SETTING, ui->_cbHideStatusbrInFullscreen->isChecked());
}
