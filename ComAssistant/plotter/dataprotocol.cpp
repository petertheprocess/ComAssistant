#include "dataprotocol.h"

DataProtocol::DataProtocol()
{
//    packsBuff = new PackStream_t;
//    dataPool = new DataPool_t;
    MAXDATA_AS_END.append(static_cast<char>(0x00));
    MAXDATA_AS_END.append(static_cast<char>(0x00));
    MAXDATA_AS_END.append(static_cast<char>(0x80));
    MAXDATA_AS_END.append(static_cast<char>(0x7F));
    clearBuff();
}

DataProtocol::~DataProtocol()
{
//    delete packsBuff;
//    delete dataPool;
}

void DataProtocol::setProtocolType(ProtocolType_e type, bool clearbuff)
{
    protocolType = type;
    if(clearbuff)
        clearBuff();
}

DataProtocol::ProtocolType_e DataProtocol::getProtocolType()
{
    return protocolType;
}

void DataProtocol::clearBuff()
{
    packsBuff.clear();
    dataPool.clear();
    unparasedBuff.clear();
}

void DataProtocol::printBuff()
{
    int i = 0;
    QString tmp2;
    foreach(RowData_t rowData, dataPool){
        tmp2.clear();
        foreach(OneData_t onedata, rowData){
            tmp2 += QString::number(onedata) + " ";
        }
        qDebug()<<"line"+QString::number(i++)+":" + tmp2;
    }
}

int DataProtocol::parasedBuffSize()
{
    return dataPool.size();
}

QVector<double> DataProtocol::popOneRowData()
{
    QVector<double> tmp;
    if(dataPool.size()>0){
        tmp = dataPool.at(0);
        dataPool.pop_front();
    }
    return tmp;
}

void DataProtocol::parase(QByteArray inputArray)
{
    RowData_t rowData;
    Pack_t pack;
    QByteArray restArray;
    unparasedBuff += inputArray;
    extractPacks(unparasedBuff, restArray);
    unparasedBuff = restArray;
    while (packsBuff.size()>0) {
        pack = popOnePack();
        rowData = extractRowData(pack);
        addToDataPool(rowData);
    }

}

void DataProtocol::extractPacks(const QByteArray &inputArray, QByteArray &restArray)
{
    if(protocolType == Ascii){
        QRegularExpression reg;
        QRegularExpressionMatch match;
        int index = 0;
        reg.setPattern("\\{.*:.+\\}");//匹配{}间的数据
        reg.setPatternOptions(QRegularExpression::InvertedGreedinessOption);//设置为非贪婪模式匹配
        do {
                match = reg.match(inputArray, index);
                if(match.hasMatch()) {
                    index = match.capturedEnd();
                    packsBuff << match.captured(0).toUtf8();
    //                qDebug()<<"match"<<match.captured(0);
                }
                else{
    //                qDebug()<<"no match";
                    break;
                }
        } while(index < inputArray.length());

        restArray = inputArray.mid(index);

    }else if(protocolType == Float){
        QByteArray tmpArray = inputArray;
        while (tmpArray.indexOf(MAXDATA_AS_END)!=-1) {
            QByteArray before = tmpArray.mid(0,tmpArray.indexOf(MAXDATA_AS_END));
            tmpArray = tmpArray.mid(tmpArray.indexOf(MAXDATA_AS_END)+MAXDATA_AS_END.size());
            packsBuff << before;
        }
        restArray = tmpArray;
    }
}

DataProtocol::Pack_t DataProtocol::popOnePack()
{
    Pack_t tmp;
    if(packsBuff.isEmpty()){
        return tmp;
    }
    tmp = packsBuff.at(0);
    packsBuff.pop_front();
    return tmp;
}

DataProtocol::RowData_t DataProtocol::extractRowData(const Pack_t &pack)
{

    RowData_t rowData;
    if(pack.isEmpty())
        return rowData;

    if(protocolType == Ascii){

        QRegularExpression reg;
        QRegularExpressionMatch match;
        int index = 0;
        reg.setPattern("[\\+-]?\\d+\\.?\\d*");//匹配实数 符号出现0、1次，数字至少1次，小数点0、1次，小数不出现或出现多次
        do {
                match = reg.match(pack, index);
                if(match.hasMatch()) {
                    index = match.capturedEnd();
                    rowData << match.captured(0).toDouble();
    //                qDebug()<<match.captured(0).toDouble();
                }
                else{
    //                qDebug()<<"no match";
                    break;
                }
        } while(index < pack.length());


    }else if(protocolType == Float){
        Pack_t tmpPack = pack;
        while (tmpPack.size()>0) {
            float tmp;
            if(packToFloat(tmpPack, tmp)){
                tmpPack = tmpPack.mid(4);
                rowData << static_cast<double>(tmp);
//                qDebug("tmp:%f",tmp);
            }else{
//                qDebug()<<"get max";
            }
        }
    }

    return rowData;
}

void DataProtocol::addToDataPool(const RowData_t& rowData)
{
    dataPool << rowData;
}

bool DataProtocol::byteArrayToFloat(const QByteArray& array, float& result)
{
    if(array.size()<3)
        return false;

    char num[4];
    for(int i = 0; i<4; i++)
        num[i] = array.at(i);//
//    qDebug("%.2f", *(reinterpret_cast<float*>(num)));

    result = *(reinterpret_cast<float*>(num));
    return true;
}

bool DataProtocol::packToFloat(const Pack_t& pack , float& result)
{
    return byteArrayToFloat(pack, result);
}
