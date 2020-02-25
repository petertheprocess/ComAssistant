#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QSerialPort>
#include <QString>
#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QTextCodec>

#define SAVE_PATH   "config.ini"
#define DEFAULT_SEND_INTERVAL   100
//节
#define SECTION_SERIAL   QString("Serial/")
#define SECTION_GLOBAL   QString("Global/")
//键
#define KEY_BAUDRATE        QString("Baudrate")
#define KEY_STOPBIT         QString("StopBit")
#define KEY_DATABIT         QString("DataBit")
#define KEY_PARITY          QString("Parity")
#define KEY_FLOWCONTROL     QString("FlowControl")
//键
#define KEY_CODERULE        QString("CodeRule")
#define KEY_ENTERSTYLE      QString("EnterStyle")
#define KEY_TIMESTAMPSTATE  QString("TimeStampState")
#define KEY_SENDINTERVAL    QString("SendInterval")
#define KEY_HEXSENDSTATE    QString("HexSendState")
#define KEY_HEXSHOWSTATE    QString("HexShowState")

//值
enum CodeRule_e{
    UTF8 = 0,
    GBK2312 = 1
};

enum EnterStyle_e{
    WinStyle = 0,
    UnixStyle = 1
};

class Config
{
public:
    Config();
    static void writeDefault(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);

        iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200);
        iniFile->setValue(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity);
        iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8);
        iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop);
        iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl);

        iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::UTF8);
        iniFile->setValue(SECTION_GLOBAL+KEY_ENTERSTYLE , EnterStyle_e::WinStyle);
        iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, false);
        iniFile->setValue(SECTION_GLOBAL+KEY_SENDINTERVAL, DEFAULT_SEND_INTERVAL);
        iniFile->setValue(SECTION_GLOBAL+KEY_HEXSENDSTATE, false);
        iniFile->setValue(SECTION_GLOBAL+KEY_HEXSHOWSTATE, false);

        delete iniFile;
    }
    static void createDefaultIfNotExist()
    {
        //文件不存在或者为空时都重建ini文件
        if(!isFileExist(SAVE_PATH)){
            writeDefault();
        }else {
            QFile file(SAVE_PATH);
            if(file.size()==0){
                writeDefault();
            }
        }
    }
    static bool isFileExist(QString path)
    {
        QFileInfo fileInfo(path);
        if(fileInfo.isFile()){
            return true;
        }
        return false;
    }

    //serial
    static void setBaudrate(int baud){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, baud);
        delete iniFile;
    }
    static void setParity(QSerialPort::Parity parity){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_SERIAL+KEY_PARITY, parity);
        delete iniFile;
    }
    static void setDataBits(QSerialPort::DataBits databits){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, databits);
        delete iniFile;
    }
    static void setStopBits(QSerialPort::StopBits stopbits){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, stopbits);
        delete iniFile;
    }
    static void setFlowControl(QSerialPort::FlowControl flowControl){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, flowControl);
        delete iniFile;
    }

    static int getBaudrate(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200).toInt();
        delete iniFile;
        return value;
    }
    static QSerialPort::Parity getParity(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity).toInt();
        delete iniFile;
        return static_cast<QSerialPort::Parity>(value);
    }
    static QSerialPort::DataBits getDataBits(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8).toInt();
        delete iniFile;
        return static_cast<QSerialPort::DataBits>(value);
    }
    static QSerialPort::StopBits getStopBits(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop).toInt();
        delete iniFile;
        return static_cast<QSerialPort::StopBits>(value);
    }
    static QSerialPort::FlowControl getFlowControl(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl).toInt();
        delete iniFile;
        return static_cast<QSerialPort::FlowControl>(value);
    }

    //general
    static void setCodeRule(CodeRule_e rule){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, rule);
        delete iniFile;
    }
    static void setEnterStyle(EnterStyle_e style){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_ENTERSTYLE , style);
        delete iniFile;
    }
    static void setTimeStampState(bool checked){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, checked);
        delete iniFile;
    }
    static void setSendInterval(const int interval){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_SENDINTERVAL, interval);
        delete iniFile;
    }
    static void setHexSendState(bool checked){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_HEXSENDSTATE, checked);
        delete iniFile;
    }
    static void setHexShowState(bool checked){
        createDefaultIfNotExist();
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        iniFile->setValue(SECTION_GLOBAL+KEY_HEXSHOWSTATE, checked);
        delete iniFile;
    }

    static CodeRule_e getCodeRule(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::UTF8).toInt();
        delete iniFile;
        return static_cast<CodeRule_e>(value);
    }
    static EnterStyle_e getEnterStyle(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_GLOBAL+KEY_CODERULE, EnterStyle_e::WinStyle).toInt();
        delete iniFile;
        return static_cast<EnterStyle_e>(value);
    }
    static bool getTimeStampState(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        bool value = iniFile->value(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, false).toBool();
        delete iniFile;
        return value;
    }
    static int getSendInterval(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        int value = iniFile->value(SECTION_GLOBAL+KEY_SENDINTERVAL, DEFAULT_SEND_INTERVAL).toInt();
        delete iniFile;
        return value;
    }
    static bool getHexSendState(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        bool value = iniFile->value(SECTION_GLOBAL+KEY_HEXSENDSTATE, false).toBool();
        delete iniFile;
        return value;
    }
    static bool getHexShowState(){
        QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
        bool value = iniFile->value(SECTION_GLOBAL+KEY_HEXSHOWSTATE, false).toBool();
        delete iniFile;
        return value;
    }

};

#endif // CONFIG_H
