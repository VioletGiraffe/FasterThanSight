#include "candroidnative.h"
#include "../../app/src/ccontroller.h"
#include "compiler/compiler_warnings_control.h"

DISABLE_COMPILER_WARNINGS
#include <QDebug>
#include <QString>

#include <jni.h>
RESTORE_COMPILER_WARNINGS

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_VioletGiraffe_FasterThanSight_FasterThanSightActivity_openTextFile(JNIEnv *env, jobject /*obj*/, jstring filePath)
{
	const char *utf8FilePath = env->GetStringUTFChars(filePath, nullptr);

	const QString filePathString = QString::fromUtf8(utf8FilePath);
	if (CAndroidNativeHelper::instance())
	{
		qDebug() << "Attempting to open file" << filePathString;
		CAndroidNativeHelper::instance()->controller().openFile(filePathString, 0);
	}

	env->ReleaseStringUTFChars(filePath, utf8FilePath);
}

#ifdef __cplusplus
}
#endif

CAndroidNativeHelper* CAndroidNativeHelper::_instance = nullptr;

CAndroidNativeHelper::CAndroidNativeHelper(CController &controller) : _controller(controller)
{
	_instance = this;
}

CAndroidNativeHelper::~CAndroidNativeHelper()
{
	_instance = nullptr;
}

CController &CAndroidNativeHelper::controller()
{
	return _controller;
}

CAndroidNativeHelper *CAndroidNativeHelper::instance()
{
	return _instance;
}
