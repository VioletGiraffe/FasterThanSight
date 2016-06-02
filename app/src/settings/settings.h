#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS


#define PIVOT_CALCULATION_METHOD QStringLiteral("Settings/Pivot/CalculationMethod")
#define DEFAULT_PIVOT_CALCULATION_METHOD TextFragment::pcmMagic
