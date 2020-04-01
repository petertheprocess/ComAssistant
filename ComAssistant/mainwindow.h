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
#include <QDesktopServices>
//网络类
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QNetworkInterface>
//绘图器类
#include "qcustomplot.h"
#include "dataprotocol.h"
#include "qcustomplotcontrol.h"
#include "axistag.h"
#include "mytracer.h"
//自定义类
#include "myxlsx.h"
#include "highlighter.h"
#include "myserialport.h"
#include "baseconversion.h"
#include "config.h"
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
    bool tryOpenSerialIfOnlyOne();

    void on_comSwitch_clicked(bool checked);

    void on_sendButton_clicked();

    void on_clearWindows_clicked();

    void readSerialPort();
    void serialBytesWritten(qint64 bytes);
    void printToTextBrowser();
    void cycleSendTimerSlot();
    void cycleReadTimerSlot();

    void on_cycleSendCheck_clicked(bool checked);

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

    void on_actionSaveShowedData_triggered();

    void on_actionUpdate_triggered();

    void on_actionSTM32_ISP_triggered();

    void on_actionMultiString_triggered(bool checked);

    void on_baudrateList_currentTextChanged(const QString &arg1);

    void on_comList_textActivated(const QString &arg1);

    void on_sendInterval_textChanged(const QString &arg1);

    void on_multiString_itemDoubleClicked(QListWidgetItem *item);

    void on_multiString_customContextMenuRequested(const QPoint &pos);
    void editSeedSlot();
    void deleteSeedSlot();
    void clearSeedsSlot();

    void on_actionPlotterSwitch_triggered(bool checked);

    void secTimerSlot();
    void debugTimerSlot();

    void on_actionAscii_triggered(bool checked);

    void on_actionFloat_triggered(bool checked);

    void on_actiondebug_triggered(bool checked);

    void verticalScrollBarActionTriggered(int action);

    //绘图器交互
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void removeSelectedGraph();
    void removeAllGraphs();
    void hideSelectedGraph();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void showTracer(QMouseEvent *event);

    void on_actionLinePlot_triggered();

    void on_actionScatterLinePlot_triggered();

    void on_actionScatterPlot_triggered();

    void on_actionResetDefaultConfig_triggered(bool checked);

    void on_actionManual_triggered();

    void on_actionSavePlotData_triggered();

    void on_actionSavePlotAsPicture_triggered();

    void httpFinishedSlot(QNetworkReply*);
    void on_actionKeyWordHighlight_triggered(bool checked);

    void on_actionUsageStatistic_triggered();

    void on_actionSendFile_triggered();

    void on_actionGBK_triggered(bool checked);

private:
    bool needSaveConfig = true;
    void readConfig();
    Ui::MainWindow *ui;
    mySerialPort serial;

    QLabel *statusSpeedLabel, *statusStatisticLabel, *statusAdLabel;

    bool paraseFile = false;
    QByteArrayList paraseFileBuff;    //解析文件分包缓冲
    int paraseFileBuffIndex = 0;

    QByteArray RxBuff, TxBuff; //原始数据的收发缓冲
    QByteArray hexBrowserBuff; //十六进制格式的浏览器缓冲
    int hexBrowserBuffIndex = 0;
    QByteArray BrowserBuff; //浏览器缓冲
    int BrowserBuffIndex = 0; //显示指示

    QByteArrayList SendFileBuff;    //发送文件分包缓冲
    int SendFileBuffIndex = 0; //
    QByteArray unshowedRxBuff;    //未上屏的接收缓冲
    QTimer cycleSendTimer; //循环发送定时器
    QTimer debugTimer; //调试定时器
    QTimer secTimer;  //秒定时器
    QTimer cycleReadTimer; //
    double rxSpeedKB = 0;
    double txSpeedKB = 0;
    int statisticRxByteCnt = 0;
    int statisticTxByteCnt = 0;

    QString enter;
    QString lastFileDialogPath;

    Highlighter *highlighter = nullptr;
    DataProtocol* protocol;
    QCustomPlotControl plotControl;
    MyTracer *m_Tracer; //坐标跟随鼠标
    //http访问
    QNetworkAccessManager *m_NetManger;
    QNetworkReply* m_Reply;
    typedef enum{
        Idle,
        GetVersion,
        BackStageGetVersion,//后台检查更新
        DownloadFile,
        PostStatic,
        DownloadADs,
    }HttpFunction_e;
//    HttpFunction_e httpFunction = Idle;//http的用途
    QVector<HttpFunction_e> httpTaskVector;
    int httpTimeout = 0;
    QStringList adList;//广告列表
    //使用统计
    int currentRunTime = 0; //运行时间

    bool postUsageStatistic(void);
    bool getRemoteVersion(void);
    bool downloadAdvertisement(void);

    bool saveGraphAsTxt(const QString& filePath, char separate=' ');
};

#endif // MAINWINDOW_H
