#pragma once

#include "settingsui/csettingspage.h"

namespace Ui {
class CSettingsPagePauses;
}

class CSettingsPagePauses : public CSettingsPage
{
public:
	explicit CSettingsPagePauses(QWidget *parent = 0);
	~CSettingsPagePauses();

	void acceptSettings() override;

private:
	Ui::CSettingsPagePauses *ui;
};
