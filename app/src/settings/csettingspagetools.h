#pragma once

#include "settingsui/csettingspage.h"

namespace Ui {
	class CSettingsPageTools;
}

class CSettingsPageTools : public CSettingsPage
{
public:
	explicit CSettingsPageTools(QWidget *parent = 0);
	~CSettingsPageTools();

	void acceptSettings() override;

private:
	Ui::CSettingsPageTools* ui;
};

