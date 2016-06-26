#pragma once

#include "settingsui/csettingspage.h"

namespace Ui {
class CSettingsPageInterface;
}

class CSettingsPageInterface : public CSettingsPage
{
public:
	explicit CSettingsPageInterface(QWidget *parent = 0);
	~CSettingsPageInterface();

	void acceptSettings() override;

private:
	Ui::CSettingsPageInterface *ui;
};
