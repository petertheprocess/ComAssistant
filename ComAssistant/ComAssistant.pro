#-------------------------------------------------
#
# Project created by QtCreator 2020-02-12T15:01:57
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
# 支持http等网络操作
QT       += network
# 支持xlsx操作
QT       += gui-private
# 支持OpenGL
DEFINES += QCUSTOMPLOT_USE_OPENGL
LIBS += -lOpengl32 \
        -lglu32

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
    QXlsx/myxlsx.cpp \
    QXlsx/source/xlsxabstractooxmlfile.cpp \
    QXlsx/source/xlsxabstractsheet.cpp \
    QXlsx/source/xlsxcell.cpp \
    QXlsx/source/xlsxcellformula.cpp \
    QXlsx/source/xlsxcelllocation.cpp \
    QXlsx/source/xlsxcellrange.cpp \
    QXlsx/source/xlsxcellreference.cpp \
    QXlsx/source/xlsxchart.cpp \
    QXlsx/source/xlsxchartsheet.cpp \
    QXlsx/source/xlsxcolor.cpp \
    QXlsx/source/xlsxconditionalformatting.cpp \
    QXlsx/source/xlsxcontenttypes.cpp \
    QXlsx/source/xlsxdatavalidation.cpp \
    QXlsx/source/xlsxdatetype.cpp \
    QXlsx/source/xlsxdocpropsapp.cpp \
    QXlsx/source/xlsxdocpropscore.cpp \
    QXlsx/source/xlsxdocument.cpp \
    QXlsx/source/xlsxdrawing.cpp \
    QXlsx/source/xlsxdrawinganchor.cpp \
    QXlsx/source/xlsxformat.cpp \
    QXlsx/source/xlsxmediafile.cpp \
    QXlsx/source/xlsxnumformatparser.cpp \
    QXlsx/source/xlsxrelationships.cpp \
    QXlsx/source/xlsxrichstring.cpp \
    QXlsx/source/xlsxsharedstrings.cpp \
    QXlsx/source/xlsxsimpleooxmlfile.cpp \
    QXlsx/source/xlsxstyles.cpp \
    QXlsx/source/xlsxtheme.cpp \
    QXlsx/source/xlsxutility.cpp \
    QXlsx/source/xlsxworkbook.cpp \
    QXlsx/source/xlsxworksheet.cpp \
    QXlsx/source/xlsxzipreader.cpp \
    QXlsx/source/xlsxzipwriter.cpp \
    dialog/about_me_dialog.cpp \
    dialog/settings_dialog.cpp \
    dialog/stm32isp_dialog.cpp \
    plotter/axistag.cpp \
    plotter/dataprotocol.cpp \
    plotter/myqcustomplot.cpp \
    plotter/mytracer.cpp \
    plotter/qcustomplot.cpp \
    plotter/qcustomplotcontrol.cpp \
    sources/baseconversion.cpp \
    sources/config.cpp \
    sources/highlighter.cpp \
    sources/http.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp \
    sources/myserialport.cpp

HEADERS += \
    QXlsx/header/xlsxabstractooxmlfile.h \
    QXlsx/header/xlsxabstractooxmlfile_p.h \
    QXlsx/header/xlsxabstractsheet.h \
    QXlsx/header/xlsxabstractsheet_p.h \
    QXlsx/header/xlsxcell.h \
    QXlsx/header/xlsxcell_p.h \
    QXlsx/header/xlsxcellformula.h \
    QXlsx/header/xlsxcellformula_p.h \
    QXlsx/header/xlsxcelllocation.h \
    QXlsx/header/xlsxcellrange.h \
    QXlsx/header/xlsxcellreference.h \
    QXlsx/header/xlsxchart.h \
    QXlsx/header/xlsxchart_p.h \
    QXlsx/header/xlsxchartsheet.h \
    QXlsx/header/xlsxchartsheet_p.h \
    QXlsx/header/xlsxcolor_p.h \
    QXlsx/header/xlsxconditionalformatting.h \
    QXlsx/header/xlsxconditionalformatting_p.h \
    QXlsx/header/xlsxcontenttypes_p.h \
    QXlsx/header/xlsxdatavalidation.h \
    QXlsx/header/xlsxdatavalidation_p.h \
    QXlsx/header/xlsxdatetype.h \
    QXlsx/header/xlsxdocpropsapp_p.h \
    QXlsx/header/xlsxdocpropscore_p.h \
    QXlsx/header/xlsxdocument.h \
    QXlsx/header/xlsxdocument_p.h \
    QXlsx/header/xlsxdrawing_p.h \
    QXlsx/header/xlsxdrawinganchor_p.h \
    QXlsx/header/xlsxformat.h \
    QXlsx/header/xlsxformat_p.h \
    QXlsx/header/xlsxglobal.h \
    QXlsx/header/xlsxmediafile_p.h \
    QXlsx/header/xlsxnumformatparser_p.h \
    QXlsx/header/xlsxrelationships_p.h \
    QXlsx/header/xlsxrichstring.h \
    QXlsx/header/xlsxrichstring_p.h \
    QXlsx/header/xlsxsharedstrings_p.h \
    QXlsx/header/xlsxsimpleooxmlfile_p.h \
    QXlsx/header/xlsxstyles_p.h \
    QXlsx/header/xlsxtheme_p.h \
    QXlsx/header/xlsxutility_p.h \
    QXlsx/header/xlsxworkbook.h \
    QXlsx/header/xlsxworkbook_p.h \
    QXlsx/header/xlsxworksheet.h \
    QXlsx/header/xlsxworksheet_p.h \
    QXlsx/header/xlsxzipreader_p.h \
    QXlsx/header/xlsxzipwriter_p.h \
    QXlsx/myxlsx.h \
    dialog/about_me_dialog.h \
    dialog/settings_dialog.h \
    dialog/stm32isp_dialog.h \
    plotter/axistag.h \
    plotter/dataprotocol.h \
    plotter/myqcustomplot.h \
    plotter/mytracer.h \
    plotter/qcustomplot.h \
    plotter/qcustomplotcontrol.h \
    sources/baseconversion.h \
    sources/config.h \
    sources/highlighter.h \
    sources/http.h \
    sources/mainwindow.h \
    sources/myserialport.h

FORMS += \
        ui/about_me_dialog.ui \
        ui/mainwindow.ui \
        ui/settings_dialog.ui \
        ui/stm32isp_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = logo.ico

#头文件搜索路径
INCLUDEPATH += $$PWD/sources/
INCLUDEPATH += $$PWD/plotter/
INCLUDEPATH += $$PWD/dialog/
INCLUDEPATH += $$PWD/QXlsx/
INCLUDEPATH += $$PWD/QXlsx/header/

# Use Precompiled headers (PCH)
#CONFIG += cmdline precompile_header
#PRECOMPILED_HEADER = stable.h

# 支持QCustomPlot
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

RESOURCES += \
    resources/resources.qrc

# 翻译文件
TRANSLATIONS = resources/en_US.ts

DISTFILES +=
