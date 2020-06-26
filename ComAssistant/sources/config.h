#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>
#include <QDateTime>
#include <QFont>
#include <QColor>

//保存路径
#define SAVE_PATH   "ComAssistantConfig.ini"
//默认发送间隔
#define DEFAULT_SEND_INTERVAL   100
//节
#define SECTION_GLOBAL    QString("Global/")
#define SECTION_SERIAL    QString("Serial/")
#define SECTION_MULTISTR  QString("MultiStr/")
#define SECTION_PLOTTER   QString("Plotter/")
#define SECTION_STATIC    QString("Static/")
#define SECTION_ABOUT     QString("About/")

//global键
#define KEY_FIRSTRUN            QString("FirstRun")
#define KEY_CODERULE            QString("CodeRule")
#define KEY_ENTERSTYLE          QString("EnterStyle")
#define KEY_TIMESTAMPSTATE      QString("TimeStampState")
#define KEY_SENDINTERVAL        QString("SendInterval")
#define KEY_HEXSENDSTATE        QString("HexSendState")
#define KEY_HEXSHOWSTATE        QString("HexShowState")
#define KEY_HIGHLIGHTSTATE      QString("HighlightState")
#define KEY_TEXTSENDAREA        QString("TextSendArea")
#define KEY_LASTFILEDIALOGPATH  QString("LastFileDialogPath")
#define KEY_TIMESTAMP_TIMEOUT   QString("TimeStampTimeOut")
#define KEY_GUIFONT             QString("GUIFont")
#define KEY_BACKGROUNDCOLOR     QString("BackGroudColor")

//serial键
#define KEY_PORTNAME        QString("PortName")
#define KEY_BAUDRATE        QString("Baudrate")
#define KEY_STOPBIT         QString("StopBit")
#define KEY_DATABIT         QString("DataBit")
#define KEY_PARITY          QString("Parity")
#define KEY_FLOWCONTROL     QString("FlowControl")

//multiStr键
#define KEY_MULTISTRINGSTATE    QString("MultiStringState")
#define KEY_MULTISTRING         QString("MultiString")

//plotter键
#define KEY_PLOTTERSTATE        QString("PlotterState")
#define KEY_PROTOCOLTYPE        QString("ProtocolType")
#define KEY_GRAPHNAME           QString("GraphName")
#define KEY_XAXISNAME           QString("XAxisName")
#define KEY_YAXISNAME           QString("YAxisName")
#define KEY_VALUEDISPLAYSTATE   QString("ValueDisplayState")
#define KEY_OPENGLSTATE         QString("OpenGLState")
//#define KEY_LINETYPE            QString("LineType")
//#define KEY_XRANGELENGH         QString("xRangeLength")

//static键
#define KEY_STARTTIME       QString("StartTime")
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
    GBK,
    UTF8
};

enum EnterStyle_e{
    WinStyle = 0,
    UnixStyle = 1
};

enum ProtocolType_e{
    Ascii = 0,
    Ascii_SumCheck,
    Float,
    Float_SumCheck,
};

extern int32_t version_to_number(QString str);

class Config
{
public:
    #define defualtGraphName  "Graph 1;Graph 2;Graph 3;Graph 4;Graph 5;Graph 6;Graph 7;Graph 8;Graph 9;Graph 10;Graph 11;Graph 12;Graph 13;Graph 14;Graph 15;"
    //版本
    #define VERSION_STRING  "0.2.8"

    Config();
    static void writeDefault();
    static void createDefaultIfNotExist();
    static bool isFileExist(QString path);

    static void setFirstRun(bool flag);
    static bool getFirstRun();

    static void setVersion(void);
    static QString getVersion();
    static int32_t getVersionNumber();
    //serial
    static void setPortName(QString name);
    static QString getPortName();
    static void setBaudrate(int baud);
    static int getBaudrate();
    static void setParity(QSerialPort::Parity parity);
    static QSerialPort::Parity getParity();
    static void setDataBits(QSerialPort::DataBits databits);
    static QSerialPort::DataBits getDataBits();
    static void setStopBits(QSerialPort::StopBits stopbits);
    static QSerialPort::StopBits getStopBits();
    static void setFlowControl(QSerialPort::FlowControl flowControl);
    static QSerialPort::FlowControl getFlowControl();

    //global
    static void setCodeRule(CodeRule_e rule);
    static CodeRule_e getCodeRule();
    static void setEnterStyle(EnterStyle_e style);
    static EnterStyle_e getEnterStyle();
    static void setTimeStampState(bool checked);
    static bool getTimeStampState();
    static void setTimeStampTimeOut(int32_t timeout);
    static int32_t getTimeStampTimeOut();
    static void setSendInterval(const int interval);
    static int getSendInterval();
    static void setHexSendState(bool checked);
    static bool getHexSendState();
    static void setHexShowState(bool checked);
    static bool getHexShowState();
    static void setMultiStringState(bool checked);
    static bool getMultiStringState();
    static bool setMultiString(QStringList multiStr);
    static QStringList getMultiString();
    static void setKeyWordHighlightState(bool checked);
    static bool getKeyWordHighlightState();
    static void setTextSendArea(QString str);
    static QString getTextSendArea();
    static void setLastFileDialogPath(QString str);
    static QString getLastFileDialogPath();
    static void setGUIFont(QFont font);
    static QFont getGUIFont();
    static void setBackGroundColor(QColor color);
    static QColor getBackGroundColor();

    //plotter
    static void setPlotterState(bool checked);
    static bool getPlotterState();
    static void setPlotterType(ProtocolType_e type);
    static ProtocolType_e getPlotterType();
    static void setPlotterGraphNames(QVector<QString> names);
    static QVector<QString> getPlotterGraphNames(int maxValidGraphNumber);
    static void setXAxisName(QString str);
    static QString getXAxisName();
    static void setYAxisName(QString str);
    static QString getYAxisName();
    static void setValueDisplayState(bool isOn);
    static bool getValueDisplayState();
    static void setOpengGLState(bool isOn);
    static bool getOpengGLState();

    //static
    static void setStartTime(QString time);
    static QString getStartTime(void);
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
