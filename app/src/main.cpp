#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "QML/creaderview.h"
#include "logger.h"

#if defined Q_OS_ANDROID || defined Q_OS_IOS
#define Q_OS_MOBILE
#endif

#ifndef Q_OS_MOBILE
#include "cmainwindow.h"
#endif

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>

#ifdef Q_OS_MOBILE
#include <QQmlApplicationEngine>
#endif

RESTORE_COMPILER_WARNINGS

QtMessageHandler standardMessageHandler = nullptr;

inline void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	if (standardMessageHandler)
		standardMessageHandler(type, context, msg);

	loggerInstance().log(msg);
}

int main(int argc, char *argv[])
{
	DISABLE_COMPILER_WARNINGS
	Q_INIT_RESOURCE(app_resources);
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

#ifdef Q_OS_MOBILE
	QQmlApplicationEngine engine;
	engine.load(QUrl(QLatin1String("qrc:/main.qml")));
#else
	CMainWindow w;
	w.show();
#endif

	return app.exec();
}
