#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDialog>
RESTORE_COMPILER_WARNINGS

namespace Ui {
class CLogViewer;
}

class CLogViewer : public QDialog
{
public:
	explicit CLogViewer(QWidget *parent = 0);
	~CLogViewer();

private:
	Ui::CLogViewer *ui;
};
