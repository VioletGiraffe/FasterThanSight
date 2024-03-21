#pragma once
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

#define READER_READING_SPEED_SETTING QStringLiteral("Reader/Reading/Speed")
#define READER_READING_SPEED_DEFAULT 300

#define READER_WORD_REWIND_SETTING QStringLiteral("Reader/Word/Rewind")
#define READER_WORD_REWIND_DEFAULT 5

#define PAUSE_SCALING_SETTING QStringLiteral("Reader/Reading/PauseScaling")
