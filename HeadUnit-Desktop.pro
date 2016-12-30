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

#Change this when cross-compling. More info here : https://wiki.debian.org/Multiarch/Tuples
TUPLE = $$system(if hash "gcc" 2>/dev/null; then gcc -print-multiarch; else dpkg-architecture -qDEB_HOST_MULTIARCH; fi)

unix: LIBS += -lssl
unix: LIBS += -lcrypto
unix: LIBS += -lusb-1.0
unix: LIBS += -lglib-2.0
unix: LIBS += -lgobject-2.0
unix: LIBS += -lgstapp-1.0
unix: LIBS += -lgstreamer-1.0
unix: LIBS += -lgstapp-1.0
unix: LIBS += -L/usr/lib/$$TUPLE/lib -lQt5GLib-2.0
unix: LIBS += -L/usr/lib/$$TUPLE/lib -lQt5GStreamer-1.0
unix: LIBS += -L/usr/lib/$$TUPLE/lib -lQt5GStreamerUi-1.0
unix: LIBS += -L/usr/lib/$$TUPLE/lib -lQt5GStreamerUtils-1.0

unix: INCLUDEPATH += /usr/include/openssl
unix: INCLUDEPATH += /usr/include/libusb-1.0
unix: INCLUDEPATH += /usr/lib/$$TUPLE
unix: INCLUDEPATH += /usr/lib/$$TUPLE/glib-2.0/include
unix: INCLUDEPATH += /usr/include/gstreamer-1.0
unix: INCLUDEPATH += /usr/include/glib-2.0
unix: INCLUDEPATH += /usr/local/include/Qt5GStreamer
unix: INCLUDEPATH += /usr/lib/$$TUPLE/include/Qt5GStreamer
unix: INCLUDEPATH += /usr/lib/$$TUPLE/glib-2.0/include

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







