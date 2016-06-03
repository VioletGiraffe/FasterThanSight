#pragma once

#include "settingsui/csettingspage.h"

namespace Ui {
class CSettingsPagePivot;
}

class CSettingsPagePivot : public CSettingsPage
{
public:
	explicit CSettingsPagePivot(QWidget *parent = 0);
	~CSettingsPagePivot();

	void acceptSettings() override;

protected:
	void resizeEvent(QResizeEvent* e) override;

private:
	Ui::CSettingsPagePivot *ui;
};
