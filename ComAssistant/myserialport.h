#ifndef MYSERIALPORT_H
#define MYSERIALPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QString>
#include <QByteArray>

class mySerialPort : public QSerialPort
{
public:
    mySerialPort();
    ~mySerialPort();
    using QSerialPort::open;
    bool open(QString PortName,int BaudRate);
    qint64 write(QByteArray data);
    QByteArray readAll();
    QList<QString> refreshSerialPort();
    unsigned int getTxCnt();
    unsigned int getRxCnt();
    QString getTxRxString();
    void resetCnt();
    void resetTxCnt();
    void resetRxCnt();
private:
//    QSerialPort *serial = new QSerialPort();
    uint32_t TxCnt;
    uint32_t RxCnt;
};

#endif // MYSERIALPORT_H
