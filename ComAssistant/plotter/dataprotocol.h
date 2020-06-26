#ifndef DATAPROTOCOL_H
#define DATAPROTOCOL_H

#include <QByteArray>
#include <QtDebug>
#include <QString>
#include <QRegularExpression>
#include <QVector>
#include <float.h>
#include <QElapsedTimer>

//可视协议：{保留:数据1,数据2,...}
//透传协议: float数据转为小端模式传输，以00 00 80 7F结尾

//pack包定义：满足可视协议的数据包为pack包，典型特征为符号{和：和}和数据

class DataProtocol
{
public:
    typedef enum{
        Ascii,
        Float
    }ProtocolType_e;
    //定义数据模型
    typedef double OneData_t;
    typedef QVector<OneData_t> RowData_t;
    typedef QVector<RowData_t> DataPool_t;
    //定义数据包和数据包流
    typedef QByteArray          Pack_t;
    typedef QVector<QByteArray> PackStream_t;

public:
    DataProtocol();
    DataProtocol(ProtocolType_e type):protocolType(type){
        DataProtocol();
    }
    ~DataProtocol();
    void printBuff();
    void clearBuff();
    int parsedBuffSize();//判断数据池剩余大小
    QVector<double> popOneRowData();//弹出一行数据，没有数据则为空
    int32_t parse(const QByteArray& inputArray, int32_t &startPos, int32_t maxParseLengthLimit);
    void setProtocolType(ProtocolType_e type, bool clearbuff=true);
    ProtocolType_e getProtocolType();
    QVector<QByteArray> getExtrackedPacks(QByteArray &inputArray);

private:
    //从输入参数1中提取所有pack包流进packsBuff缓存中
    void extractPacks(QByteArray &inputArray, QByteArray &restArray, bool toDataPool);
    //从pack缓存中弹出一个pack
    Pack_t popOnePack();
    //从pack中提取合法数据行
    RowData_t extractRowData(const Pack_t& pack);
    //将合法数据行添加进数据池
    void addToDataPool(const RowData_t& rowData);
    //pack缓存、数据池
    PackStream_t packsBuff;
    DataPool_t dataPool;
    QByteArray unparsedBuff;
    //协议类型
    ProtocolType_e protocolType = Ascii;
    //最大常数
    QByteArray MAXDATA_AS_END;
    //数组前4个字节转float
    bool byteArrayToFloat(const QByteArray& array, float& result);
    //调用byteArrayToFloat
    bool packToFloat(const Pack_t& pack , float& result);
};

#endif // DATAPROTOCOL_H
