#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextCodec>
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
#include <QDataStream>
#include <QFile>
#include <QRegExp>
#include <QListWidgetItem>
#include <QDesktopWidget>
#include <QVector>
#include <QMenu>
#include <QInputDialog>
#include <QList>
//绘图器类
#include "myqcustomplot.h"
#include "dataprotocol.h"
//自定义类
#include "myxlsx.h"
#include "highlighter.h"
#include "myserialport.h"
#include "baseconversion.h"
#include "config.h"
#include "http.h"
//界面类
#include "stm32isp_dialog.h"
#include "about_me_dialog.h"
#include "settings_dialog.h"

#define PAGEING_SIZE_MAX 4068
static int PAGING_SIZE = 4068; //TextBrowser分页显示大小，这个值正好满屏显示

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void paraseFileSignal();
private slots:
    void paraseFileSlot();

    void on_refreshCom_clicked();
    void tryOpenSerial();
    void readSerialPort();
    void serialBytesWritten(qint64 bytes);
    void handleSerialError(QSerialPort::SerialPortError errCode);
    
    void printToTextBrowser();

    //main gui
    void on_comSwitch_clicked(bool checked);
    void on_sendButton_clicked();
    void on_clearWindows_clicked();
    void on_cycleSendCheck_clicked(bool checked);
    void on_textEdit_textChanged();
    void on_hexSend_stateChanged(int arg1);
    void on_hexDisplay_clicked(bool checked);
    void on_baudrateList_currentTextChanged(const QString &arg1);
    void on_comList_textActivated(const QString &arg1);
    void on_sendInterval_textChanged(const QString &arg1);
    void on_multiString_itemDoubleClicked(QListWidgetItem *item);
    void verticalScrollBarActionTriggered(int action);
    
    //file
    void on_actionSaveOriginData_triggered();
    void on_actionOpenOriginData_triggered();
    void on_actionSaveShowedData_triggered();
    void on_actionSavePlotData_triggered();
    void on_actionSavePlotAsPicture_triggered();    
    void on_actionSendFile_triggered();

    //function
    void on_actionMultiString_triggered(bool checked);
    void on_actionSTM32_ISP_triggered();
    
    //setting
    void on_actionCOM_Config_triggered();
    void on_actionGBK_triggered(bool checked);
    void on_actionUTF8_triggered(bool checked);
    void on_action_winLikeEnter_triggered(bool checked);
    void on_action_unixLikeEnter_triggered(bool checked);
    void on_actionKeyWordHighlight_triggered(bool checked);
    void on_actionResetDefaultConfig_triggered(bool checked);
    
    //visualization
    void on_actionPlotterSwitch_triggered(bool checked);
    void on_actionLinePlot_triggered();
    void on_actionScatterLinePlot_triggered();
    void on_actionScatterPlot_triggered();
    void on_actionValueDisplay_triggered(bool checked);
    void on_actionAscii_triggered(bool checked);
    void on_actionFloat_triggered(bool checked);
    void on_actiondebug_triggered(bool checked);

    //help
    void on_actionManual_triggered();
    void on_actionUsageStatistic_triggered();
    void on_actionUpdate_triggered();
    void on_actionAbout_triggered();

    //timer
    void secTimerSlot();
    void debugTimerSlot();
    void cycleSendTimerSlot();
    void cycleReadTimerSlot();

    //contextMenuRequested
    void on_textBrowser_customContextMenuRequested(const QPoint &pos);
    void clearTextBrowserSlot();

    void on_valueDisplay_customContextMenuRequested(const QPoint &pos);
    void deleteValueDisplayRowSlot();
    void deleteValueDisplaySlot();

    void on_multiString_customContextMenuRequested(const QPoint &pos);
    void editSeedSlot();
    void deleteSeedSlot();
    void clearSeedsSlot();

    void on_timeStampCheckBox_stateChanged(int arg1);

    void on_timeOut_textChanged(const QString &arg1);

private:
    bool needSaveConfig = true;
    void readConfig();
    Ui::MainWindow *ui;
    mySerialPort serial;

    QLabel *statusSpeedLabel, *statusStatisticLabel, *statusRemoteMsgLabel; //状态栏标签

    bool paraseFile = false;
    QByteArrayList paraseFileBuff;    //解析文件分包缓冲
    int paraseFileBuffIndex = 0;
    QByteArrayList SendFileBuff;    //发送文件分包缓冲
    int SendFileBuffIndex = 0; //

    QByteArray RxBuff, TxBuff; //原始数据的收发缓冲
    QByteArray hexBrowserBuff; //十六进制格式的浏览器缓冲
    int hexBrowserBuffIndex = 0;
    QByteArray BrowserBuff; //浏览器缓冲
    int BrowserBuffIndex = 0; //显示指示
    QByteArray unshowedRxBuff;    //未上屏的接收缓冲

    QTimer cycleSendTimer;  //循环发送定时器
    QTimer debugTimer;      //调试定时器
    QTimer secTimer;        //秒定时器
    QTimer cycleReadTimer;  //周期读取定时器
    QTimer timeStampTimer;  //时间戳定时器

    QString enter;
    QString lastFileDialogPath; //上次文件对话框路径

    Highlighter *highlighter = nullptr; //高亮器

    DataProtocol* protocol;
    QCustomPlotControl* plotControl;

    HTTP *http;

    bool printToBrowserFlag = true; //滚屏开关

    //统计
    int currentRunTime = 0; //运行时间
    double rxSpeedKB = 0;
    double txSpeedKB = 0;
    int statisticRxByteCnt = 0;
    int statisticTxByteCnt = 0;
};

#endif // MAINWINDOW_H
