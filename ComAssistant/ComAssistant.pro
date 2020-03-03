#-------------------------------------------------
#
# Project created by QtCreator 2020-02-12T15:01:57
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ComAssistant
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

SOURCES += \
        baseconversion.cpp \
        config.cpp \
        dialog/about_me_dialog.cpp \
        dialog/settings_dialog.cpp \
        dialog/stm32isp_dialog.cpp \
        highlighter.cpp \
        main.cpp \
        mainwindow.cpp \
        myserialport.cpp \
        plotter/axistag.cpp \
        plotter/dataprotocol.cpp \
    plotter/mytracer.cpp \
        plotter/qcustomplot.cpp \
        plotter/qcustomplotcontrol.cpp \

HEADERS += \
        baseconversion.h \
        config.h \
        dialog/about_me_dialog.h \
        dialog/settings_dialog.h \
        dialog/stm32isp_dialog.h \
        highlighter.h \
        mainwindow.h \
        myserialport.h \
        plotter/axistag.h \
        plotter/dataprotocol.h \
    plotter/mytracer.h \
        plotter/qcustomplot.h \
        plotter/qcustomplotcontrol.h \

FORMS += \
        about_me_dialog.ui \
        mainwindow.ui \
        settings_dialog.ui \
        stm32isp_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = logo.ico

#头文件搜索路径
INCLUDEPATH += $$PWD/plotter/
INCLUDEPATH += $$PWD/dialog/

# Use Precompiled headers (PCH)
#CONFIG += cmdline precompile_header
#PRECOMPILED_HEADER = stable.h

# 支持QCustomPlot
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
