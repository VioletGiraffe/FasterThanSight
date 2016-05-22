#pragma once
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

#define UI_FONT_SIZE_SETTING QStringLiteral("UI/Font/Size")
#define UI_FONT_SIZE_DEFAULT 24

#define UI_FONT_FAMILY QStringLiteral("UI/Font/Family")
#define UI_FONT_STYLE QStringLiteral("UI/Font/Style")
#define UI_FONT_WEIGHT QStringLiteral("UI/Font/Weight")

#define UI_OPEN_FILE_LAST_USED_DIR_SETTING QStringLiteral("UI/File/OpenFileDir")

#define UI_SHOW_PIVOT_SETTING QStringLiteral("UI/Reading/ShowPivot")
#define UI_SHOW_PIVOT_DEFAULT false
