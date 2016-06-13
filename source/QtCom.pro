#-------------------------------------------------
#
# Project created by QtCreator 2013-09-02T22:53:34
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtCom
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    qextserialport/qextserialport.cpp \
    mytextedit.cpp \
    optiondialog.cpp \
    mythread.cpp

win32 {
    SOURCES += qextserialport/qextserialport_win.cpp
}

unix {
    SOURCES += qextserialport/qextserialport_unix.cpp
}

HEADERS  += mainwindow.h \
    qextserialport/qextserialport_p.h \
    qextserialport/qextserialport_global.h \
    qextserialport/qextserialport.h \
    mytextedit.h \
    optiondialog.h \
    mythread.h

FORMS    += mainwindow.ui \
    optiondialog.ui

OTHER_FILES +=
