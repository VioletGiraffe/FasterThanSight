#include "cmainwindow.h"

#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "QML/creaderview.h"
#include "logger.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>
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

	CMainWindow w;
	w.show();

	return app.exec();
}
