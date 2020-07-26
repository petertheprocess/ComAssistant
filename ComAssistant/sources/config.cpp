#include "config.h"

int32_t version_to_number(QString str)
{
    QStringList list = str.split(".");
    if(list.size()!=3)
        return -1;

    int32_t number = 0;
    number = list.at(0).toInt()*65536 + list.at(1).toInt()*256 + list.at(2).toInt();
    return number;
}

Config::Config()
{

}

void Config::writeDefault(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);

    QFont defaultFont;
    defaultFont.setFamilies(QStringList()<<"Courier New"<<"Consolas"<<"Microsoft YaHei UI");
    defaultFont.setPointSize(10);
    QColor defaultColor(Qt::white);

    iniFile->setValue(SECTION_GLOBAL+KEY_FIRSTRUN, true);
    iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::GBK);
    iniFile->setValue(SECTION_GLOBAL+KEY_ENTERSTYLE , EnterStyle_e::WinStyle);
    iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMPSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_SENDINTERVAL, DEFAULT_SEND_INTERVAL);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSENDSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_HEXSHOWSTATE, false);
    iniFile->setValue(SECTION_GLOBAL+KEY_HIGHLIGHTSTATE, true);
    iniFile->setValue(SECTION_GLOBAL+KEY_TEXTSENDAREA, "");
    iniFile->setValue(SECTION_GLOBAL+KEY_LASTFILEDIALOGPATH, "");
    iniFile->setValue(SECTION_GLOBAL+KEY_GUIFONT, defaultFont);
    iniFile->setValue(SECTION_GLOBAL+KEY_BACKGROUNDCOLOR, defaultColor);
    iniFile->setValue(SECTION_GLOBAL+KEY_POPUPHOTKEY, "Shift+Alt+C");

    iniFile->setValue(SECTION_SERIAL+KEY_PORTNAME, "");
    iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200);
    iniFile->setValue(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity);
    iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8);
    iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop);
    iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl);

    iniFile->setValue(SECTION_MULTISTR+KEY_MULTISTRINGSTATE, false);

    iniFile->setValue(SECTION_PLOTTER+KEY_PLOTTERSTATE, false);
    iniFile->setValue(SECTION_PLOTTER+KEY_PROTOCOLTYPE, ProtocolType_e::Ascii);
    iniFile->setValue(SECTION_PLOTTER+KEY_GRAPHNAME, defualtGraphName);
    iniFile->setValue(SECTION_PLOTTER+KEY_XAXISNAME, "Point Number");
    iniFile->setValue(SECTION_PLOTTER+KEY_YAXISNAME, "Value");
    iniFile->setValue(SECTION_PLOTTER+KEY_VALUEDISPLAYSTATE, false);

    //统计信息
//    iniFile->setValue(SECTION_PLOTTER+KEY_STARTTIME, QDateTime::currentDateTime().toString("yyyyMMddhhmmss"));
//    iniFile->setValue(SECTION_STATIC+KEY_LASTRUNTIME, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_TOTALRUNTIME, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_LASTTXCNT, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_TOTALTXCNT, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_LASTRXCNT, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_TOTALRXCNT, 0);
//    iniFile->setValue(SECTION_STATIC+KEY_TOTALRUNCNT, 0);

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

void Config::setFirstRun(bool flag){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_FIRSTRUN, flag);
    delete iniFile;
}
bool Config::getFirstRun(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_FIRSTRUN, true).toBool();
    delete iniFile;
    return value;
}

void Config::setVersion(void){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_ABOUT+KEY_VERSION, VERSION_STRING);
    delete iniFile;
}
QString Config::getVersion(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_ABOUT+KEY_VERSION, VERSION_STRING).toString();
    delete iniFile;
    return value;
}
int32_t Config::getVersionNumber()
{
    return version_to_number(getVersion());
}
//serial
void Config::setPortName(QString name){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_PORTNAME, name);
    delete iniFile;
}
QString Config::getPortName(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_SERIAL+KEY_PORTNAME, "").toString();
    delete iniFile;
    return value;
}

void Config::setBaudrate(int baud){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_BAUDRATE, baud);
    delete iniFile;
}
int Config::getBaudrate(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_BAUDRATE, QSerialPort::Baud115200).toInt();
    delete iniFile;
    return value;
}

void Config::setParity(QSerialPort::Parity parity){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_PARITY, parity);
    delete iniFile;
}
QSerialPort::Parity Config::getParity(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_PARITY, QSerialPort::NoParity).toInt();
    delete iniFile;
    return static_cast<QSerialPort::Parity>(value);
}

void Config::setDataBits(QSerialPort::DataBits databits){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_DATABIT, databits);
    delete iniFile;
}
QSerialPort::DataBits Config::getDataBits(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_DATABIT, QSerialPort::Data8).toInt();
    delete iniFile;
    return static_cast<QSerialPort::DataBits>(value);
}

void Config::setStopBits(QSerialPort::StopBits stopbits){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_STOPBIT, stopbits);
    delete iniFile;
}
QSerialPort::StopBits Config::getStopBits(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_STOPBIT, QSerialPort::OneStop).toInt();
    delete iniFile;
    return static_cast<QSerialPort::StopBits>(value);
}

void Config::setFlowControl(QSerialPort::FlowControl flowControl){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_SERIAL+KEY_FLOWCONTROL, flowControl);
    delete iniFile;
}
QSerialPort::FlowControl Config::getFlowControl(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_SERIAL+KEY_FLOWCONTROL, QSerialPort::NoFlowControl).toInt();
    delete iniFile;
    return static_cast<QSerialPort::FlowControl>(value);
}

//global
void Config::setCodeRule(CodeRule_e rule){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_CODERULE, rule);
    delete iniFile;
}
CodeRule_e Config::getCodeRule(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int value = iniFile->value(SECTION_GLOBAL+KEY_CODERULE, CodeRule_e::GBK).toInt();
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

void Config::setTimeStampTimeOut(int32_t timeout){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_TIMESTAMP_TIMEOUT, timeout);
    delete iniFile;
}
int32_t Config::getTimeStampTimeOut(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    int32_t value = iniFile->value(SECTION_GLOBAL+KEY_TIMESTAMP_TIMEOUT, 20).toInt();
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

bool Config::setMultiString(QStringList multiStr){

    if(multiStr.isEmpty()){
        multiStr.append("发送的数据会自动添加进条目");
        multiStr.append("双击条目发送");
        multiStr.append("右击条目删除/编辑");
    }

    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);

    int i = 0;
    for(i = 0; i > -1; i++){
        QString tmp = iniFile->value(SECTION_MULTISTR+KEY_MULTISTRING+QString::number(i), "").toString();
        if(!tmp.isEmpty()){
            iniFile->setValue(SECTION_MULTISTR+KEY_MULTISTRING+QString::number(i), "");
        }else{
            break;
        }
    }
    for(i = 0; i < multiStr.size(); i++){
        iniFile->setValue(SECTION_MULTISTR+KEY_MULTISTRING+QString::number(i), multiStr.at(i));
    }

    delete iniFile;
    return true;
}

QStringList Config::getMultiString(){
    QStringList res;
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    for(int i = 0; i > -1; i++){
        QString preStr;
        switch (i) {
            case 0: preStr = "发送的数据会自动添加进条目"; break;
            case 1: preStr = "双击条目发送"; break;
            case 2: preStr = "右击条目删除/编辑"; break;
            default: preStr = ""; break;
        }
        QString value = iniFile->value(SECTION_MULTISTR+KEY_MULTISTRING+QString::number(i), preStr).toString();
        if(value.isEmpty())
            break;
        else
            res.append(value);
    }
    delete iniFile;
    return res;
}

void Config::setKeyWordHighlightState(bool checked){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_HIGHLIGHTSTATE, checked);
    delete iniFile;
}
bool Config::getKeyWordHighlightState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_GLOBAL+KEY_HIGHLIGHTSTATE, true).toBool();
    delete iniFile;
    return value;
}

void Config::setTextSendArea(QString str){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_TEXTSENDAREA, str);
    delete iniFile;
}
QString Config::getTextSendArea(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_GLOBAL+KEY_TEXTSENDAREA, "").toString();
    delete iniFile;
    return value;
}

void Config::setLastFileDialogPath(QString str){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_LASTFILEDIALOGPATH, str);
    delete iniFile;
}
QString Config::getLastFileDialogPath(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_GLOBAL+KEY_LASTFILEDIALOGPATH, "").toString();
    delete iniFile;
    return value;
}

void Config::setGUIFont(QFont font){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_GUIFONT, font);
    delete iniFile;
}
QFont Config::getGUIFont(){
    QFont defaultFont;
    defaultFont.setFamilies(QStringList()<<"Courier New"<<"Consolas"<<"Microsoft YaHei UI");
    defaultFont.setPointSize(10);
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QFont font = qvariant_cast<QFont>(iniFile->value(SECTION_GLOBAL+KEY_GUIFONT, defaultFont));
    delete iniFile;
    return font;
}

void Config::setBackGroundColor(QColor color){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_BACKGROUNDCOLOR, color);
    delete iniFile;
}
QColor Config::getBackGroundColor(){
    QColor defaultColor(Qt::white);
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QColor color = qvariant_cast<QColor>(iniFile->value(SECTION_GLOBAL+KEY_BACKGROUNDCOLOR, defaultColor));
    delete iniFile;
    return color;
}
void Config::setPopupHotKey(QString keySequence)
{
    if(keySequence.isEmpty())
        keySequence = "Shift+Alt+C";
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_GLOBAL+KEY_POPUPHOTKEY, keySequence);
    delete iniFile;
}
QString Config::getPopupHotKey()
{
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_GLOBAL+KEY_POPUPHOTKEY, "Shift+Alt+C").toString();
    delete iniFile;
    return value;
}

//plotter
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

void Config::setXAxisName(QString str){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_XAXISNAME, str);
    delete iniFile;
}
QString Config::getXAxisName(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_PLOTTER+KEY_XAXISNAME, "Point Number").toString();
    delete iniFile;
    return value;
}

void Config::setYAxisName(QString str){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_YAXISNAME, str);
    delete iniFile;
}
QString Config::getYAxisName(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_PLOTTER+KEY_YAXISNAME, "Value").toString();
    delete iniFile;
    return value;
}

void Config::setValueDisplayState(bool isOn){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_VALUEDISPLAYSTATE, isOn);
    delete iniFile;
}
bool Config::getValueDisplayState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_PLOTTER+KEY_VALUEDISPLAYSTATE, false).toBool();
    delete iniFile;
    return value;
}

void Config::setOpengGLState(bool isOn){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_PLOTTER+KEY_OPENGLSTATE, isOn);
    delete iniFile;
}
bool Config::getOpengGLState(){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    bool value = iniFile->value(SECTION_PLOTTER+KEY_OPENGLSTATE, false).toBool();
    delete iniFile;
    return value;
}

//static
void Config::setStartTime(QString time){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_STARTTIME, time);
    delete iniFile;
}
QString Config::getStartTime(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_STARTTIME, QDateTime::currentDateTime().toString("yyyyMMddhhmmss")).toString();
    delete iniFile;
    return value;
}

void Config::setLastRunTime(int sec){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_LASTRUNTIME, sec);
    delete iniFile;
}
QString Config::getLastRunTime(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_LASTRUNTIME, 0).toString();
    delete iniFile;
    return value;
}

void Config::setTotalRunTime(int currentRunTime){
    createDefaultIfNotExist();
    int total = getTotalRunTime().toInt();
    total += currentRunTime;
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_TOTALRUNTIME, total);
    delete iniFile;
}
QString Config::getTotalRunTime(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_TOTALRUNTIME, 0).toString();
    delete iniFile;
    return value;
}

void Config::setLastTxCnt(int64_t cnt){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_LASTTXCNT, cnt);
    delete iniFile;
}
QString Config::getLastTxCnt(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_LASTTXCNT, 0).toString();
    delete iniFile;
    return value;
}

void Config::setLastRxCnt(int64_t cnt){
    createDefaultIfNotExist();
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_LASTRXCNT, cnt);
    delete iniFile;
}
QString Config::getLastRxCnt(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_LASTRXCNT, 0).toString();
    delete iniFile;
    return value;
}

void Config::setTotalTxCnt(int64_t currentTxCnt){
    createDefaultIfNotExist();
    int64_t total = getTotalTxCnt().toInt();
    total += currentTxCnt;
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_TOTALTXCNT, total);
    delete iniFile;
}
QString Config::getTotalTxCnt(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_TOTALTXCNT, 0).toString();
    delete iniFile;
    return value;
}

void Config::setTotalRxCnt(int64_t currentRxCnt){
    createDefaultIfNotExist();
    int64_t total = getTotalRxCnt().toInt();
    total += currentRxCnt;
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_TOTALRXCNT, total);
    delete iniFile;
}
QString Config::getTotalRxCnt(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_TOTALRXCNT, 0).toString();
    delete iniFile;
    return value;
}

void Config::setTotalRunCnt(int64_t runCnt){
    createDefaultIfNotExist();
    int64_t total = getTotalRunCnt().toInt();
    total += runCnt;
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    iniFile->setValue(SECTION_STATIC+KEY_TOTALRUNCNT, total);
    delete iniFile;
}
QString Config::getTotalRunCnt(void){
    QSettings *iniFile = new QSettings(SAVE_PATH, QSettings::IniFormat);
    QString value = iniFile->value(SECTION_STATIC+KEY_TOTALRUNCNT, 0).toString();
    delete iniFile;
    return value;
}
