#pragma once

#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QObject>
RESTORE_COMPILER_WARNINGS

class CController;

class CAndroidNativeHelper : public QObject
{
	Q_OBJECT

public:
	CAndroidNativeHelper(CController& controller);
	~CAndroidNativeHelper();

	CController& controller();

	static CAndroidNativeHelper* instance();

private:
	CController& _controller;

	static CAndroidNativeHelper* _instance;
};
