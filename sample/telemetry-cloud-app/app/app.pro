TARGET = telemetrycloudapp
QT += core gui gui-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES = main.cpp \
    mainwindow.cpp \
    TelemetryWorker.cpp

HEADERS  += mainwindow.h \
    ui_mainwindow.h \
    TelemetryWorker.h

CONFIG += link_pkgconfig
PKGCONFIG += libhomescreen qtappfw-core libcloudproxy

CONFIG(release, debug|release) {
    QMAKE_POST_LINK = $(STRIP) --strip-unneeded $(TARGET)
}

LIBS += -ljson-c -lafbwsc -lsystemd

RESOURCES += \
    images/images.qrc

include(app.pri)

DISTFILES += \
    images/AGL_HMI_Blue_Background_NoCar-01.png

