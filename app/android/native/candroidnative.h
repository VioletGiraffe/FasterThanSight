#pragma once

class CController;

class CAndroidNativeHelper
{
public:
	CAndroidNativeHelper(CController& controller);
	~CAndroidNativeHelper();

	CController& controller();

	static CAndroidNativeHelper* instance();

private:
	CController& _controller;

	static CAndroidNativeHelper* _instance;
};
