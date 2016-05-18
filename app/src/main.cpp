#include "cmainwindow.h"

#include "assert/advanced_assert.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AdvancedAssert::setLoggingFunc([](const char* message) {
		qDebug() << message;
	});

	CMainWindow w;
	w.show();

	return a.exec();
}
