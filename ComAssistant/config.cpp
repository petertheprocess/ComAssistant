#include "config.h"

Config::Config()
{

}

void Config::writeDefault(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);

    iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::UTF8);
    iniFile->setValue(SECTION_GLOBAL+KEY_ENTERSTYLE , EnterStyle_e::WinStyle);
    iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_SENDINTERVAL, DEFAULT_SEND_INTERVAL);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSENDSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSHOWSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_MULTISTRINGSTATE, false);

    iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200);
    iniFile->setValue(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity);
    iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8);
    iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop);
    iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl);

    iniFile->setValue(SECTION_PLOTTER+KEY_PLOTTERSTATE, false);
    iniFile->setValue(SECTION_PLOTTER+KEY_PROTOCOLTYPE, ProtocolType_e::Ascii);
    iniFile->setValue(SECTION_PLOTTER+KEY_GRAPHNAME, defualtGraphName);

    iniFile->setValue(SECTION_ABOUT+KEY_VERSION, VERSION_STRING);
    iniFile->setValue(SECTION_ABOUT+KEY_SOURCE_CODE, "www.github.com/inhowe/ComAssistant");
    iniFile->setValue(SECTION_ABOUT+KEY_AUTHER, "INHOWE");
    iniFile->setValue(SECTION_ABOUT+KEY_EMAIL, "inhowe@qq.com");

    delete iniFile;
}
void Config::createDefaultIfNotExist()
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
bool Config::isFileExist(QString path)
{
    QFileInfo fileInfo(path);
    if(fileInfo.isFile()){
        return true;
    }
    return false;
}
QString Config::getVersion(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_ABOUT+KEY_VERSION, VERSION_STRING).toString();
    delete iniFile;
    return value;
}
//serial
void Config::setBaudrate(int baud){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, baud);
    delete iniFile;
}
void Config::setParity(QSerialPort::Parity parity){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_PARITY, parity);
    delete iniFile;
}
void Config::setDataBits(QSerialPort::DataBits databits){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, databits);
    delete iniFile;
}
void Config::setStopBits(QSerialPort::StopBits stopbits){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, stopbits);
    delete iniFile;
}
void Config::setFlowControl(QSerialPort::FlowControl flowControl){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, flowControl);
    delete iniFile;
}

int Config::getBaudrate(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200).toInt();
    delete iniFile;
    return value;
}
QSerialPort::Parity Config::getParity(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity).toInt();
    delete iniFile;
    return static_cast<QSerialPort::Parity>(value);
}
QSerialPort::DataBits Config::getDataBits(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8).toInt();
    delete iniFile;
    return static_cast<QSerialPort::DataBits>(value);
}
QSerialPort::StopBits Config::getStopBits(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop).toInt();
    delete iniFile;
    return static_cast<QSerialPort::StopBits>(value);
}
QSerialPort::FlowControl Config::getFlowControl(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl).toInt();
    delete iniFile;
    return static_cast<QSerialPort::FlowControl>(value);
}

//general
void Config::setCodeRule(CodeRule_e rule){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, rule);
    delete iniFile;
}
CodeRule_e Config::getCodeRule(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::UTF8).toInt();
    delete iniFile;
    return static_cast<CodeRule_e>(value);
}

void Config::setEnterStyle(EnterStyle_e style){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_ENTERSTYLE , style);
    delete iniFile;
}
EnterStyle_e Config::getEnterStyle(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_GLOBAL+KEY_CODERULE, EnterStyle_e::WinStyle).toInt();
    delete iniFile;
    return static_cast<EnterStyle_e>(value);
}

void Config::setTimeStampState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, checked);
    delete iniFile;
}
bool Config::getTimeStampState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, false).toBool();
    delete iniFile;
    return value;
}

void Config::setSendInterval(const int interval){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_SENDINTERVAL, interval);
    delete iniFile;
}
int Config::getSendInterval(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_GLOBAL+KEY_SENDINTERVAL, DEFAULT_SEND_INTERVAL).toInt();
    delete iniFile;
    return value;
}

void Config::setHexSendState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSENDSTATE, checked);
    delete iniFile;
}
bool Config::getHexSendState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_HEXSENDSTATE, false).toBool();
    delete iniFile;
    return value;
}

void Config::setHexShowState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSHOWSTATE, checked);
    delete iniFile;
}
bool Config::getHexShowState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_HEXSHOWSTATE, false).toBool();
    delete iniFile;
    return value;
}

void Config::setMultiStringState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_MULTISTRINGSTATE, checked);
    delete iniFile;
}
bool Config::getMultiStringState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_MULTISTRINGSTATE, false).toBool();
    delete iniFile;
    return value;
}

void Config::setPlotterState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_PLOTTERSTATE, checked);
    delete iniFile;
}
bool Config::getPlotterState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_PLOTTER +KEY_PLOTTERSTATE, false).toBool();
    delete iniFile;
    return value;
}
void Config::setPlotterType(ProtocolType_e type){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_PROTOCOLTYPE, type);
    delete iniFile;
}
ProtocolType_e Config::getPlotterType(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_PLOTTER+KEY_PROTOCOLTYPE, ProtocolType_e::Ascii).toInt();
    delete iniFile;
    return static_cast<ProtocolType_e>(value);
}

void Config::setPlotterGraphNames(QVector<QString> names){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString str;
    if(names.size()>0){
      foreach(QString tmp, names){
        str = str + tmp + ";";
      }
//      qDebug()<<"setPlotterGraphNames"<<str;
      iniFile->setValue(SECTION_PLOTTER+KEY_GRAPHNAME, str);
    }
    delete iniFile;
}
QVector<QString> Config::getPlotterGraphNames(int maxValidGraphNumber){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_PLOTTER+KEY_GRAPHNAME, defualtGraphName).toString();
    QStringList tmp = value.split(';');
    QVector<QString> newValue;
    //转Vector格式
    for(int i = 0; i < tmp.size(); i++){
        newValue.append(tmp.at(i));
    }
    //多退少补
    if(newValue.size() < maxValidGraphNumber){
        for(int i = newValue.size(); i < maxValidGraphNumber; i++){
            newValue.append("Graph "+QString::number(i+1));
        }
    }else{
        while (newValue.size()>maxValidGraphNumber) {
            newValue.pop_back();
        }
    }
    delete iniFile;
//    qDebug()<<"getPlotterGraphNames:"<<newValue;
    return newValue;
}
