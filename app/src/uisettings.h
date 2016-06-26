#pragma once
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

#define UI_BRIGHTNESS QStringLiteral("UI/Screen/Brightness")
#define UI_BRIGHTNESS_DEFAULT 100

#define UI_FONT_SIZE_SETTING QStringLiteral("UI/Font/Size")
#define UI_FONT_SIZE_DEFAULT 24

#define UI_FONT_FAMILY QStringLiteral("UI/Font/Family")
#define UI_FONT_STYLE QStringLiteral("UI/Font/Style")
#define UI_FONT_WEIGHT QStringLiteral("UI/Font/Weight")

#define UI_OPEN_FILE_LAST_USED_DIR_SETTING QStringLiteral("UI/File/OpenFileDir")

#define UI_SHOW_PIVOT_SETTING QStringLiteral("UI/Reading/ShowPivot")
#define UI_SHOW_PIVOT_DEFAULT true

#define UI_CLEAR_SCREEN_AFTER_SENTENCE_END QStringLiteral("UI/Reading/FadeOutAfterSentenceEnd")
#define UI_CLEAR_SCREEN_AFTER_SENTENCE_END_DEFAULT true

#define UI_BOOKMARKS_STORAGE QStringLiteral("UI/Bookmarks/Bookrmarks")

#define UI_PIVOT_CALCULATION_METHOD QStringLiteral("Settings/Pivot/CalculationMethod")
#define UI_PIVOT_CALCULATION_METHOD_DEFAULT TextFragment::pcmMagic

#define UI_LONG_WORD_THRESHOLD_SETTING QStringLiteral("Settings/Pauses/LongWordThreshold")
#define UI_LONG_WORD_THRESHOLD_DEFAULT 7

#define UI_LONG_WORD_DELAY_FACTOR_SETTING QStringLiteral("Settings/Pauses/LongWordDelayFactor")
#define UI_LONG_WORD_DELAY_FACTOR_DEFAULT 1.05f

#define UI_MAIN_TOOLBAR_VISIBLE_SETTING QStringLiteral("UI/Toolbars/MainToolbarVisible")
#define UI_SETTINGS_TOOLBAR_VISIBLE_SETTING QStringLiteral("UI/Toolbars/SettingsToolbarVisible")

#define UI_STATUSBAR_HIDE_IN_FULLSCREEN_SETTING QStringLiteral("UI/StatusBar/VisibleInFullscreen")
#define UI_STATUSBAR_HIDE_IN_FULLSCREEN_DEFAULT false
