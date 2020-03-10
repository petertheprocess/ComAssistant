#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>

//版本
#define VERSION_STRING  "0.0.4"
//保存路径
#define SAVE_PATH   "config.ini"
//默认发送间隔
#define DEFAULT_SEND_INTERVAL   100
//节
#define SECTION_GLOBAL   QString("Global/")
#define SECTION_SERIAL   QString("Serial/")
#define SECTION_PLOTTER   QString("Plotter/")
#define SECTION_STATIC   QString("Static/")
#define SECTION_ABOUT    QString("About/")

//global键
#define KEY_CODERULE            QString("CodeRule")
#define KEY_ENTERSTYLE          QString("EnterStyle")
#define KEY_TIMESTAMPSTATE      QString("TimeStampState")
#define KEY_SENDINTERVAL        QString("SendInterval")
#define KEY_HEXSENDSTATE        QString("HexSendState")
#define KEY_HEXSHOWSTATE        QString("HexShowState")
#define KEY_MULTISTRINGSTATE    QString("MultiStringState")
#define KEY_KEYWORDHIGHLIGHTSTATE      QString("HighlightState")

//serial键
#define KEY_BAUDRATE        QString("Baudrate")
#define KEY_STOPBIT         QString("StopBit")
#define KEY_DATABIT         QString("DataBit")
#define KEY_PARITY          QString("Parity")
#define KEY_FLOWCONTROL     QString("FlowControl")

//plotter键
#define KEY_PLOTTERSTATE        QString("PlotterState")
#define KEY_PROTOCOLTYPE        QString("ProtocolType")
#define KEY_GRAPHNAME           QString("GraphName")
//#define KEY_LINETYPE            QString("LineType")
//#define KEY_XRANGELENGH         QString("xRangeLength")

//static键
#define KEY_LASTRUNTIME     QString("LastRunTime")
#define KEY_TOTALRUNTIME    QString("TotalRunTime")
#define KEY_LASTTXCNT       QString("LastTxCnt")
#define KEY_TOTALTXCNT      QString("TotalTxCnt")
#define KEY_LASTRXCNT       QString("LastRxCnt")
#define KEY_TOTALRXCNT      QString("TotalRxCnt")
#define KEY_TOTALRUNCNT     QString("TotalRunCnt")
//about键
#define KEY_VERSION     QString("Version")
#define KEY_SOURCE_CODE QString("SourceCode")
#define KEY_AUTHER      QString("Auther")
#define KEY_EMAIL       QString("Email")

//
//值
enum CodeRule_e{
    UTF8 = 0,
    GBK2312 = 1
};

enum EnterStyle_e{
    WinStyle = 0,
    UnixStyle = 1
};

enum ProtocolType_e{
    Ascii = 0,
    Float = 1
};

class Config
{
public:
    #define defualtGraphName  "Graph 1;Graph 2;Graph 3;Graph 4;Graph 5;Graph 6;Graph 7;Graph 8;Graph 9;Graph 10;Graph 11;Graph 12;Graph 13;Graph 14;Graph 15;"
    Config();
    static void writeDefault();
    static void createDefaultIfNotExist();
    static bool isFileExist(QString path);
    static QString getVersion();
    static void setBaudrate(int baud);
    static void setParity(QSerialPort::Parity parity);
    static void setDataBits(QSerialPort::DataBits databits);
    static void setStopBits(QSerialPort::StopBits stopbits);
    static void setFlowControl(QSerialPort::FlowControl flowControl);
    static int getBaudrate();
    static QSerialPort::Parity getParity();
    static QSerialPort::DataBits getDataBits();
    static QSerialPort::StopBits getStopBits();
    static QSerialPort::FlowControl getFlowControl();
    static void setCodeRule(CodeRule_e rule);
    static CodeRule_e getCodeRule();
    static void setEnterStyle(EnterStyle_e style);
    static EnterStyle_e getEnterStyle();
    static void setTimeStampState(bool checked);
    static bool getTimeStampState();
    static void setSendInterval(const int interval);
    static int getSendInterval();
    static void setHexSendState(bool checked);
    static bool getHexSendState();
    static void setHexShowState(bool checked);
    static bool getHexShowState();
    static void setMultiStringState(bool checked);
    static bool getMultiStringState();
    static void setKeyWordHighlightState(bool checked);
    static bool getKeyWordHighlightState();
    static void setPlotterState(bool checked);
    static bool getPlotterState();
    static void setPlotterType(ProtocolType_e type);
    static ProtocolType_e getPlotterType();
    static void setPlotterGraphNames(QVector<QString> names);
    static QVector<QString> getPlotterGraphNames(int maxValidGraphNumber);
    static void setLastRunTime(int sec);
    static QString getLastRunTime(void);
    static void setTotalRunTime(int sec);
    static QString getTotalRunTime(void);
    static void setLastTxCnt(int64_t cnt);
    static QString getLastTxCnt(void);
    static void setLastRxCnt(int64_t cnt);
    static QString getLastRxCnt(void);
    static void setTotalTxCnt(int64_t currentTxCnt);
    static QString getTotalTxCnt(void);
    static void setTotalRxCnt(int64_t currentRxCnt);
    static QString getTotalRxCnt(void);
    static void setTotalRunCnt(int64_t runCnt=1);
    static QString getTotalRunCnt(void);
};

#endif // CONFIG_H
