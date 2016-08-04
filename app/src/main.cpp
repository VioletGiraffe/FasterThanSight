#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "QML/creaderview.h"
#include "styling/cthemeprovider.h"
#include "logger.h"

#ifndef MOBILE_PLATFORM
#include "cmainwindow.h"
#else
#include "../android/native/candroidnative.h"
#include "ccontroller.h"
#endif

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>
#include <QStringBuilder>

#ifdef MOBILE_PLATFORM
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#endif

RESTORE_COMPILER_WARNINGS

QtMessageHandler standardMessageHandler = nullptr;

inline void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	if (standardMessageHandler)
		standardMessageHandler(type, context, msg);

	loggerInstance().log(qFormatLogMessage(type, context, msg));
}

int main(int argc, char *argv[])
{
	qSetMessagePattern("%{function} (%{file}:%{line}): %{message}");

	DISABLE_COMPILER_WARNINGS
	Q_INIT_RESOURCE(app_resources);
	#ifdef MOBILE_PLATFORM
		Q_INIT_RESOURCE(qml);
	#endif
	RESTORE_COMPILER_WARNINGS

	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	standardMessageHandler = qInstallMessageHandler(messageHandler);

	QApplication app(argc, argv);

	qmlRegisterType<CReaderView>("ReaderView", 1, 0, "CReaderView");

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	app.setOrganizationName("GitHubSoft");
	app.setApplicationName("Faster Than Sight");

	CSettings::setApplicationName(app.applicationName());
	CSettings::setOrganizationName(app.organizationName());

#ifdef MOBILE_PLATFORM
	QQuickStyle::setStyle("Material");

	qmlRegisterType<CController>("Controller", 1, 0, "CController");
	qmlRegisterType<CThemeProvider>("ThemeProvider", 1, 0, "CThemeProvider");

	QQmlApplicationEngine engine;

	CController controller;
	engine.rootContext()->setContextProperty("controller", &controller);
	engine.rootContext()->setContextProperty("themeProvider", &CThemeProvider::instance());

	engine.load(QUrl(QLatin1String("qrc:/main.qml")));

	app.connect(&app, &QApplication::applicationStateChanged, [&controller](Qt::ApplicationState state){
		if (state != Qt::ApplicationActive)
			controller.saveState();
	});

#ifdef Q_OS_ANDROID
	CAndroidNativeHelper nativeHelper(controller);
#endif

#else
	CMainWindow w;
	w.show();
#endif

	return app.exec();
}
