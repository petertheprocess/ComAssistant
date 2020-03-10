#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QString>
#include <QByteArray>

#include <config.h>

class mySerialPort : public QSerialPort
{
public:
    mySerialPort();
    ~mySerialPort();
    //保留父类的open函数并重载open函数
    using QSerialPort::open;
    bool open(QString PortName,int BaudRate);
    qint64 write(QByteArray data);
    QByteArray readAll();
    //刷新串口
    QList<QString> refreshSerialPort();
    //获取收发统计值
    unsigned int getTxCnt();
    unsigned int getRxCnt();
    int64_t getTotalTxCnt();
    int64_t getTotalRxCnt();
    QString getTxRxString();
    //重置收发统计
    void resetCnt();
    void resetTxCnt();
    void resetRxCnt();
    //更多设置，任何一个设置失败均会返回false
    bool moreSetting(StopBits stopbits=OneStop, Parity parity=NoParity,
                     FlowControl flowcontrol=NoFlowControl, DataBits databits=Data8);
private:
    uint32_t TxCnt;
    uint32_t RxCnt;
    DataBits databits;
    StopBits stopbits;
    FlowControl flowcontrol;
    Parity paritybit;
    int64_t totalTxCnt;
    int64_t totalRxCnt;
};

#endif // MYSERIALPORT_H
