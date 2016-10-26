#-------------------------------------------------
#
# Project created by QtCreator 2016-08-28T20:57:41
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HeadUnit-Desktop
TEMPLATE = app

#QMAKE_CXXFLAGS += -pthread -static-libgcc -static-libstdc++
#-Wcomment -Wunused-variable -Wunused-parameter
#QMAKE_CFLAGS += -pthread# -Wcomment -Wunused-variable -Wunused-parameter
#-lpthread #-fpermissive -lrt

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

unix: LIBS += -lcrypt
unix: LIBS += -lcrypto
unix: LIBS += -lssl
unix: LIBS += -lusb-1.0
unix: LIBS += -lglib-2.0
unix: LIBS += -lgobject-2.0
unix: LIBS += -lgstapp-1.0
unix: LIBS += -lgstreamer-1.0
unix: LIBS += -lgstapp-1.0
unix: LIBS += -lQt5GLib-2.0
unix: LIBS += -lQt5GStreamer-1.0
unix: LIBS += -L$$PWD/../../../usr/local/lib/ -lQt5GStreamerUi-1.0
unix: LIBS += -L$$PWD/../../../usr/local/lib/ -lQt5GStreamerUtils-1.0

unix: INCLUDEPATH += /usr/include
unix: INCLUDEPATH += /usr/include/libusb-1.0
unix: INCLUDEPATH += /usr/include/gstreamer-1.0
unix: INCLUDEPATH += /usr/include/glib-2.0
unix: INCLUDEPATH += /usr/local/include/Qt5GStreamer
unix: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32: LIBS += -llibeay32MD
win32: LIBS += -lssleay32MD

win32: INCLUDEPATH += $$PWD/include/win32
win32: DEPENDPATH += $$PWD/bin

win32: INCLUDEPATH += $$PWD/bin
win32: DEPENDPATH += $$PWD/bin


win32: LIBS += -L$$PWD/bin/dll/ -llibusb-1.0

win32: INCLUDEPATH += $$PWD/bin/dll
win32: DEPENDPATH += $$PWD/bin/dll


INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include






