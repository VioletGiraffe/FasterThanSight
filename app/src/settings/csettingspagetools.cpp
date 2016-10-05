#include "csettingspagetools.h"
#include "settings/csettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspagetools.h"

#include <QMessageBox>
RESTORE_COMPILER_WARNINGS

CSettingsPageTools::CSettingsPageTools(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPageTools)
{
	ui->setupUi(this);

	connect(ui->_btnWipeSettings, &QPushButton::clicked, this, [this](){
		CSettings().clear();
		QMessageBox::information(this, tr("Settings have been wiped"), tr("Restart the application for the default settings to take effect."));
	});
}

CSettingsPageTools::~CSettingsPageTools()
{
	delete ui;
}

void CSettingsPageTools::acceptSettings()
{

}
