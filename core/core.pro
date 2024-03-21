TEMPLATE = lib
TARGET   = core

QT = core
CONFIG += staticlib
CONFIG += c++2b

mac* | linux*{
	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug
}

android {
	Release:OUTPUT_DIR=android/release
	Debug:OUTPUT_DIR=android/debug

} else:ios {
	Release:OUTPUT_DIR=ios/release
	Debug:OUTPUT_DIR=ios/debug

} else {
	Release:OUTPUT_DIR=release
	Debug:OUTPUT_DIR=debug

}

DESTDIR  = ../bin/$${OUTPUT_DIR}
OBJECTS_DIR = ../build/$${OUTPUT_DIR}/core
MOC_DIR     = ../build/$${OUTPUT_DIR}/core
UI_DIR      = ../build/$${OUTPUT_DIR}/core
RCC_DIR     = ../build/$${OUTPUT_DIR}/core

# Required for qDebug() to log function name, file and line in release build
DEFINES += QT_MESSAGELOGCONTEXT

win*{
	QMAKE_CXXFLAGS += /std:c++latest /permissive-
	QMAKE_CXXFLAGS += /MP /wd4251
	QMAKE_CXXFLAGS_WARN_ON = /W4
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX _SCL_SECURE_NO_WARNINGS

	QMAKE_LFLAGS += /INCREMENTAL /DEBUG:FASTLINK
}

mac* | linux* {
	QMAKE_CFLAGS   += -pedantic-errors
	QMAKE_CXXFLAGS += -pedantic-errors
	QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-c++11-extensions -Wno-local-type-template-args -Wno-deprecated-register

	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug

	Release:DEFINES += NDEBUG=1
	Debug:DEFINES += _DEBUG
}

LIBS += -L$${DESTDIR} -lcpputils

win32*:!*msvc2012:*msvc* {
	QMAKE_CXXFLAGS += /FS
}

mac*|linux*{
	PRE_TARGETDEPS += $${DESTDIR}/libcpputils.a $${DESTDIR}/libqtutils.a
}

INCLUDEPATH += \
	../cpputils/ \
	../cpp-template-utils/ \
	../qtutils/

include (bookmarks/bookmarks.pri)
include (recentFiles/recentFiles.pri)
include (parser/parser.pri)
include (text/text.pri)
include (reader/reader.pri)

