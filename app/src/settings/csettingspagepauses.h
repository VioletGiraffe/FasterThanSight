#pragma once

#include "settingsui/csettingspage.h"
#include "text/ctextfragment.h"

#include <map>

class QDoubleSpinBox;

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

	std::map<TextFragment::Delimiter::Enum_Type, QDoubleSpinBox*> _controlForPauseValue;
};
