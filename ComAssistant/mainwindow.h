#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QtCore/QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QRegExp>
#include <QListWidgetItem>
#include <QDesktopWidget>
#include <QVector>
#include <QMenu>
#include <QInputDialog>

#include "qcustomplot.h"
#include "dataprotocol.h"
#include "qcustomplotcontrol.h"
#include "highlighter.h"
#include "myserialport.h"
#include "baseconversion.h"
#include "config.h"
//界面类
#include "stm32isp_dialog.h"
#include "about_me_dialog.h"
#include "settings_dialog.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_refreshCom_clicked();

    void on_comSwitch_clicked(bool checked);

    void on_sendButton_clicked();

    void on_clearWindows_clicked();

    void readSerialPort();

    void continuousWriteSlot();

    void autoSubcontractTimerSlot();

    void on_TimerSendCheck_clicked(bool checked);

    void on_textEdit_textChanged();

    void on_hexSend_stateChanged(int arg1);

    void on_hexDisplay_clicked(bool checked);

    void on_action_winLikeEnter_triggered(bool checked);

    void on_action_unixLikeEnter_triggered(bool checked);

    void on_actionUTF8_triggered(bool checked);

    void on_actionSaveOriginData_triggered();

    void on_actionReadOriginData_triggered();

    void on_actionAbout_triggered();

    void on_actionCOM_Config_triggered();

    void on_baudrateList_currentTextChanged(const QString &arg1);

    void on_comList_currentTextChanged(const QString &arg1);

    void on_actionSaveShowedData_triggered();

    void on_actionUpdate_triggered();

    void on_sendInterval_textChanged(const QString &arg1);

    void on_actionSTM32_ISP_triggered();

    void on_multiString_itemDoubleClicked(QListWidgetItem *item);

    void on_actionMultiString_triggered(bool checked);

    void on_multiString_customContextMenuRequested(const QPoint &pos);

    void deleteSeedSlot();

    void clearSeedsSlot();

    void on_actionPlotter_2_triggered(bool checked);

    void debugTimerSlot();

    void on_actionAscii_triggered(bool checked);

    void on_actionFloat_triggered(bool checked);

    void on_actiondebug_triggered(bool checked);

    //绘图器交互
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void removeSelectedGraph();
    void removeAllGraphs();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);

    void on_actionLinePlot_triggered();

    void on_actionScatterLinePlot_triggered();

    void on_actionScatterPlot_triggered();

private:
    void readConfig();
    Ui::MainWindow *ui;
    mySerialPort serial;
    QLabel *statusLabel1, *statusLabel2;
    QByteArray RxBuff, TxBuff; //原始数据的收发缓冲
    QByteArray unshowedRxBuff;    //未上屏的接收缓冲
    QTimer continuousWriteTimer; //自动发送定时器
    QTimer autoSubcontractTimer; //自动分包定时器
    QTimer debugTimer; //调试定时器
    Highlighter *highlighter;
    DataProtocol* protocol;
    QCustomPlotControl plotControl;
};

#endif // MAINWINDOW_H
