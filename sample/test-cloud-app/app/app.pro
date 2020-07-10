TARGET = testcloudapp
QT += core gui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_INSTANCE_NUM='\\"1\\"'

SOURCES = main.cpp \
    mainwindow.cpp \

HEADERS  += mainwindow.h \
    ui_mainwindow.h \

CONFIG += link_pkgconfig
PKGCONFIG += libcloudproxy qtappfw-core

CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $(STRIP) --strip-unneeded $(TARGET)
}

LIBS += -ljson-c -lafbwsc -lsystemd

RESOURCES += \
    images/images.qrc

include(app.pri)

DISTFILES += \
    images/AGL_HMI_Blue_Background_NoCar-01.png

