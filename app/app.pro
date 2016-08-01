QT = core gui network quickwidgets

android*|ios*{
	CONFIG += mobile

	DEFINES += MOBILE_PLATFORM
}

!mobile{
	QT += widgets
} else {
	QT += quick
}

TARGET = FasterThanSight
TEMPLATE = app

CONFIG += c++14

mac* | linux*{
	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug
}

Release:OUTPUT_DIR=release
Debug:OUTPUT_DIR=debug

DESTDIR  = ../bin/$${OUTPUT_DIR}
OBJECTS_DIR = ../build/$${OUTPUT_DIR}/app
MOC_DIR     = ../build/$${OUTPUT_DIR}/app
UI_DIR      = ../build/$${OUTPUT_DIR}/app
RCC_DIR     = ../build/$${OUTPUT_DIR}/app

LIBS += -L../bin/$${OUTPUT_DIR} -lcore -lcpputils -lqtutils

!mobile{
	LIBS += -lautoupdater

	!win*{
		PRE_TARGETDEPS += $${DESTDIR}/libautoupdater.a
	}
}

win*{
	QMAKE_CXXFLAGS += /MP /wd4251
	QMAKE_CXXFLAGS_WARN_ON = /W4
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX _SCL_SECURE_NO_WARNINGS
}

linux*|mac*{

	QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-c++11-extensions -Wno-local-type-template-args -Wno-deprecated-register

	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug

	Release:DEFINES += NDEBUG=1
	Debug:DEFINES += _DEBUG

	PRE_TARGETDEPS += $${DESTDIR}/libcore.a
}

win32*:!*msvc2012:*msvc* {
	QMAKE_CXXFLAGS += /FS
}

win*{
	RC_FILE = resources/FTS.rc
}

mac*{
	ICON = resources/fts_icon.icns
}

INCLUDEPATH += \
	src \
	../core \
	../cpputils \
	../cpp-template-utils \
	../qtutils \
	../github-releases-autoupdater/src

include (src/widgets/widgets.pri)
include (src/QML/QML.pri)

SOURCES += \
	src/main.cpp \
	src/styling/cthemeprovider.cpp \
	src/ccontroller.cpp \
    android/native/candroidnative.cpp

HEADERS += \
	src/uisettings.h \
	src/version.h \
	src/uihelpers.h \
	src/logger.h \
	src/styling/cthemeprovider.h \
	src/ccontroller.h \
    android/native/candroidnative.h

RESOURCES += \
	src/app_resources.qrc

mobile{
	RESOURCES += \
		src/mobile/qml.qrc

	android{
		DISTFILES += \
			android/AndroidManifest.xml \
			android/gradle/wrapper/gradle-wrapper.jar \
			android/gradlew \
			android/res/values/libs.xml \
			android/build.gradle \
			android/gradle/wrapper/gradle-wrapper.properties \
			android/gradlew.bat \
			android/src/VioletGiraffe/FasterThanSight/FasterThanSightActivity.java

			HEADERS +=

			SOURCES +=

		ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
	}

} else {
	include (src/settings/settings.pri)

	SOURCES += \
		src/cmainwindow.cpp \
		src/bookmarks/cbookmarkseditor.cpp \
		src/styling/cthemesdialog.cpp \
		src/logviewer/clogviewer.cpp

	HEADERS += \
		src\cmainwindow.h \
		src/bookmarks/cbookmarkseditor.h \
		src/styling/cthemesdialog.h \
		src/logviewer/clogviewer.h

	FORMS += \
		src\cmainwindow.ui \
		src/bookmarks/cbookmarkseditor.ui \
		src/styling/cthemesdialog.ui \
		src/logviewer/clogviewer.ui
}
