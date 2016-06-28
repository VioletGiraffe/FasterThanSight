#include "csettingspagepauses.h"
#include "reader/cpausehandler.h"

#include "settings/csettings.h"
#include "../uisettings.h"

DISABLE_COMPILER_WARNINGS
#include "ui_csettingspagepauses.h"

#include <QDoubleSpinBox>
RESTORE_COMPILER_WARNINGS

CSettingsPagePauses::CSettingsPagePauses(QWidget *parent) :
	CSettingsPage(parent),
	ui(new Ui::CSettingsPagePauses)
{
	ui->setupUi(this);

	CSettings s;
	ui->_sbLongWordThreshold->setValue(s.value(UI_LONG_WORD_THRESHOLD_SETTING, UI_LONG_WORD_THRESHOLD_DEFAULT).toInt());
	ui->_sbLongWordDelayFactor->setValue(s.value(UI_LONG_WORD_DELAY_FACTOR_SETTING, UI_LONG_WORD_DELAY_FACTOR_DEFAULT).toDouble());

	QGridLayout * pauseControlLayout = new QGridLayout(ui->_pauseScalingGroupbox);
	pauseControlLayout->setColumnStretch(2, 2);
	QHBoxLayout * pauseControlGroupLayout = new QHBoxLayout(ui->_pauseScalingGroupbox);
	pauseControlGroupLayout->addLayout(pauseControlGroupLayout);
	pauseControlGroupLayout->addStretch(0);
	ui->_pauseScalingGroupbox->setLayout(pauseControlGroupLayout);

	int row = 0;
	// One could iterate over CPauseHandler::pauseScalingValues() instead.
	// But iterating over all the Delimiter values ensures an assertion when there's a mismatch between the enum values list and the CPauseHandler.
	for (const auto delimiter: TextFragment::Delimiter())
	{
		if (delimiter == TextFragment::NoDelimiter)
			continue;

		pauseControlLayout->addWidget(new QLabel(QString::fromStdString(delimiter.name), ui->_pauseScalingGroupbox), row, 0);

		QDoubleSpinBox * sb = new QDoubleSpinBox(ui->_pauseScalingGroupbox);
		pauseControlLayout->addWidget(sb, row, 1);
		sb->setSingleStep(0.01);
		sb->setDecimals(2);
		sb->setValue(CPauseHandler::instance().pauseFactorForDelimiter(delimiter));

		_controlForPauseValue[delimiter.id] = sb;

		++row;
	}
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

	CPauseHandler& pauseHandler = CPauseHandler::instance();
	for (const auto& pauseScaleItem: _controlForPauseValue)
		pauseHandler.updatePauseFactor(pauseScaleItem.first, (float)pauseScaleItem.second->value());

	pauseHandler.storeToSettings();
}
