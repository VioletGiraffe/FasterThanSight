#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS


#define PIVOT_CALCULATION_METHOD QStringLiteral("Settings/Pivot/CalculationMethod")
#define DEFAULT_PIVOT_CALCULATION_METHOD TextFragment::pcmMagic

#define FADE_OUT_AFTER_SENTENCE_END QStringLiteral("Settings/ReaderView/FadeOutAfterSentenceEnd")
#define FADE_OUT_AFTER_SENTENCE_END_DEFAULT true
