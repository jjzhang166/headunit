#-------------------------------------------------
#
# Project created by QtCreator 2016-08-28T20:57:41
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HeadUnit-Desktop
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    headunit/hu_ssl.c \
    headunit/hu_usb.c \
    headunit/hu_uti.c \
    headunit/hu_platform_specific.c \
    headunit/hu_aap.c \
    headunit/hu_gst.c \
    clickablevideowidget.cpp

HEADERS  += mainwindow.h \
    headunit/hu_ssl.h \
    headunit/hu_usb.h \
    headunit/hu_uti.h \
    headunit/hu_oap.h \
    headunit/hu_aap.h \
    headunit/hu_buffers.h \
    headunit/hu_gst.h \
    clickablevideowidget.h

FORMS    += mainwindow.ui

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

CONFIG += link_pkgconfig
PKGCONFIG += libssl libcrypto libusb-1.0 glib-2.0 gobject-2.0 gstreamer-1.0 gstreamer-app-1.0 Qt5GLib-2.0 Qt5GStreamer-1.0 Qt5GStreamerUi-1.0 Qt5GStreamerUtils-1.0

win32: LIBS += -llibeay32MD
win32: LIBS += -lssleay32MD
win32: INCLUDEPATH += $$PWD/include/win32
win32: DEPENDPATH += $$PWD/bin
win32: INCLUDEPATH += $$PWD/bin
win32: DEPENDPATH += $$PWD/bin
win32: INCLUDEPATH += $$PWD/bin/dll
win32: DEPENDPATH += $$PWD/bin/dll
win32: LIBS += -L$$PWD/bin/dll/ -llibusb-1.0

INCLUDEPATH += /usr/local/include
DEPENDPATH += /usr/local/include







