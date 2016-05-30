#include "cmainwindow.h"

#include "assert/advanced_assert.h"
#include "settings/csettings.h"
#include "styling/ccolorsdialog.h"

DISABLE_COMPILER_WARNINGS
#include <QApplication>
#include <QDebug>
RESTORE_COMPILER_WARNINGS

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	app.setOrganizationName("GitHubSoft");
	app.setApplicationName("Faster Than Sight");

	CSettings::setApplicationName(app.applicationName());
	CSettings::setOrganizationName(app.organizationName());

	CMainWindow w;
	w.show();

	app.setStyleSheet(CColorsDialog::currentAcceptedStyle());

	return app.exec();
}
