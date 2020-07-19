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
    QSerialPort TmpSerial;

    //搜索串口
    foreach (const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        TmpSerial.setPort(info);
        if(TmpSerial.open(QSerialPort::ReadWrite))
        {
            TmpSerial.close();
            tmp.append(TmpSerial.portName()+"("+tr("空闲:")+info.description()+")");
        }
        else
        {
            TmpSerial.close();
            tmp.append(TmpSerial.portName()+"("+tr("占用:")+info.description()+")");
        }
    }

    //排序
    QList<QString> sorted;
    if(false == tmp.isEmpty()){
        QList<QString> tmp2;//用于存放COM0-COM9的条目
        QList<QString> tmp3;//用于存放COM10-COM99的条目
        for(int i = 0; i < tmp.size(); i++){
            //COM号在0-9
            if(tmp.at(i).indexOf("(")==4){
                tmp2.append(tmp.at(i));
            }
            else if(tmp.at(i).indexOf("(")==5){
                tmp3.append(tmp.at(i));
            }

        }
        tmp2.sort();
        tmp3.sort();
        sorted.append(tmp2);
        sorted.append(tmp3);
    }else{
        sorted = tmp;
    }

    return sorted;
}

/*
 *  如果当前设备处于占用状态，又被拔出后可能无法检测到数量变化，需要用错误处理槽函数先进行处理
*/
bool mySerialPort::portAmountChanged()
{
    static int lastPortAmount = 0;
    int currentPortAmount = QSerialPortInfo::availablePorts().size();
    if(lastPortAmount != currentPortAmount){
        lastPortAmount = currentPortAmount;
        return true;
    }else{
        return false;
    }
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
