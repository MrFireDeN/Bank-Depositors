#-------------------------------------------------
#
# Project created by QtCreator 2023-03-23T12:58:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BankDepositors
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

LIBS += C:\Qt\6.7.0\mingw_64\lib\WS2_32.Lib
LIBS += -lws2_32

SOURCES += \
        deposit.cpp \
        depositDatabase.cpp \
        fiovalidator.cpp \
        main.cpp \
        mainwindow.cpp \
        randomrecord.cpp

HEADERS += \
        deposit.h \
        depositDatabase.h \
        fiovalidator.h \
        mainwindow.h \
        randomrecord.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
