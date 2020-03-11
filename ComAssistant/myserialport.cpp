#include "myserialport.h"

mySerialPort::mySerialPort():TxCnt(0),RxCnt(0),totalTxCnt(0),totalRxCnt(0)
{
    moreSetting(Config::getStopBits(),Config::getParity(),Config::getFlowControl(),Config::getDataBits());
}

mySerialPort::~mySerialPort()
{
    if(mySerialPort::isOpen())
        mySerialPort::close();
}

/*
 * Function: reset tx/rx cnt statistics
*/
void mySerialPort::resetCnt()
{
    TxCnt = 0;
    RxCnt = 0;
}

/*
 * Function: reset tx cnt statistics
*/
void mySerialPort::resetTxCnt()
{
    TxCnt = 0;
}

/*
 * Function: reset rx cnt statistics
*/
void mySerialPort::resetRxCnt()
{
    RxCnt = 0;
}

/*
 * Function: refresh serial port
 * Parameter: none
 * Return: QList<QString>
*/
QList<QString> mySerialPort::refreshSerialPort()
{
    QList<QString> tmp;

    //搜索串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        QSerialPort TmpSerial;
        TmpSerial.setPort(info);
        if(TmpSerial.open(QSerialPort::ReadWrite))
        {
            TmpSerial.close();
            tmp.append(TmpSerial.portName()+"("+"空闲:"+info.description()+")");
        }
        else
        {
            TmpSerial.close();
            tmp.append(TmpSerial.portName()+"("+"占用:"+info.description()+")");
        }
    }

    return tmp;
}

/*
 * Function: 获取发送统计
*/
unsigned int mySerialPort::getTxCnt()
{
    return TxCnt;
}

/*
*/
unsigned int mySerialPort::getRxCnt()
{
    return RxCnt;
}

/*
 * Function: 获取发送统计
*/
int64_t mySerialPort::getTotalTxCnt()
{
    return totalTxCnt;
}

/*
*/
int64_t mySerialPort::getTotalRxCnt()
{
    return totalRxCnt;
}

/*
*/
QString mySerialPort::getTxRxString()
{
    return "Tx:"+QString::number(getTxCnt())+" Rx:"+QString::number(getRxCnt());
}

/*
*/
bool mySerialPort::open(QString PortName,int BaudRate)
{
    mySerialPort::setPortName(PortName);
    mySerialPort::setBaudRate(BaudRate);
//    mySerialPort::setDataBits(mySerialPort::Data8);
//    mySerialPort::setParity(mySerialPort::NoParity);
//    mySerialPort::setStopBits(mySerialPort::OneStop);
//    mySerialPort::setFlowControl(mySerialPort::NoFlowControl);
    mySerialPort::setReadBufferSize(1024);

    return mySerialPort::open(mySerialPort::ReadWrite);
}

/*
 *
*/
qint64 mySerialPort::write(const QByteArray& data)
{
    qint64 tmp;

    tmp = QSerialPort::write(data);
    if(tmp != -1){
        TxCnt+=static_cast<uint32_t>(data.size());
        totalTxCnt+=static_cast<uint32_t>(data.size());
    }

    return tmp;
}

/*
 *
*/
QByteArray mySerialPort::readAll()
{
    QByteArray tmp;

    tmp = QSerialPort::readAll();
    if(!tmp.isEmpty()){
        RxCnt+=static_cast<uint32_t>(tmp.size());
        totalRxCnt+=static_cast<uint32_t>(tmp.size());
    }

    return tmp;
}

bool mySerialPort::moreSetting(StopBits sb, Parity pa, FlowControl fc, DataBits db)
{
    databits = db;
    stopbits = sb;
    flowcontrol = fc;
    paritybit = pa;

    return  mySerialPort::setDataBits(databits) &&
            mySerialPort::setParity(paritybit) &&
            mySerialPort::setStopBits(stopbits) &&
            mySerialPort::setFlowControl(flowcontrol);
}
