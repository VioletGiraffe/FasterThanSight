#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QString>
RESTORE_COMPILER_WARNINGS

inline QString secondsToHhhMmSs(size_t seconds, QString format = QStringLiteral("hhh:mm:ss"))
{
	const auto ss = seconds % 60;
	const auto mm = (seconds / 60) % 60;
	const auto hhh = seconds / 3600;

	return format
		.replace(QStringLiteral("hhh"), QStringLiteral("%1").arg(hhh, 2, 10, QLatin1Char('0')))
		.replace(QStringLiteral("mm"), QStringLiteral("%1").arg(mm, 2, 10, QLatin1Char('0')))
		.replace(QStringLiteral("ss"), QStringLiteral("%1").arg(ss, 2, 10, QLatin1Char('0')));
}
