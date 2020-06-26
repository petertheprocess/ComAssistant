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
    unparsedBuff.clear();
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

int DataProtocol::parsedBuffSize()
{
    return dataPool.size();
}

QVector<double> DataProtocol::popOneRowData()
{
    QVector<double> tmp;
    if(dataPool.size()>0){
        tmp = dataPool.at(0);
//        qDebug()<<"popOneRowData:"<<tmp;
        dataPool.pop_front();
    }
    return tmp;
}

/*
 * Function: 解析协议
 * para1: 被解析的数据
 * return: 已经扫描过的数据长度
*/
int32_t DataProtocol::parse(const QByteArray& inputArray, int32_t &startPos, int32_t maxParseLengthLimit=-1, bool enableSumCheck=false)
{   
    RowData_t rowData;
    Pack_t pack;
    QByteArray restArray;
    int32_t oldsize, newsize, scannedLength;

    //未解析数据，防止数据量过大，限制了最大解析长度
    oldsize = unparsedBuff.size();
    newsize = inputArray.size() - oldsize;
    if(newsize - oldsize < maxParseLengthLimit){
        unparsedBuff += inputArray.mid(startPos);
    }else{
        unparsedBuff += inputArray.mid(startPos, maxParseLengthLimit);
    }
    scannedLength = unparsedBuff.size() - oldsize;

    //数据流分包
    extractPacks(unparsedBuff, restArray, true, enableSumCheck);
    unparsedBuff = restArray;

    return scannedLength;
}

/*
 *
 * param0[in] 待提取的数据
 * param1[in] 剩余未转换数据
 * param2[in] 直接转换到数据池中，减少数据拷贝过程
*/
inline void DataProtocol::extractPacks(QByteArray &inputArray, QByteArray &restArray, bool toDataPool=false, bool enableSumCheck=false)
{
    if(protocolType == Ascii){
        //先剔除\0,\r,\n等特殊数据
        inputArray = inputArray.trimmed();
        while (inputArray.indexOf('\0')!=-1) {
            inputArray.remove(inputArray.indexOf('\0'), 1);
        }
        QRegularExpression reg;
        QRegularExpressionMatch match;
        int index = 0;
        //匹配{}间的数据。
        //{:之间不允许再出现{:
        //:后，数据与逗号作为一个组，这个组至少出现一次，组中的逗号出现0次或1次，组开头允许有空白字符\\s
        //组中的数据：符号出现或者不出现，整数部分出现至少1次，小数点与小数作为整体，可不出现或者1次
        //换行符最多出现2次
        reg.setPattern("\\{[^{:]*:(\\s*([+-]?\\d+(\\.\\d+)?)?,?)+\\}");
        reg.setPatternOptions(QRegularExpression::InvertedGreedinessOption);//设置为非贪婪模式匹配
        do {
                QByteArray tmp;
                match = reg.match(inputArray, index);
                if(match.hasMatch()) {
                    index = match.capturedEnd();
                    //连续的逗号和分号逗号之间补0
                    tmp.clear();
                    tmp.append(match.captured(0).toLocal8Bit());
                    while(tmp.indexOf(",,")!=-1){
                        tmp.insert(tmp.indexOf(",,")+1,'0');
                    }
                    while(tmp.indexOf(":,")!=-1){
                        tmp.insert(tmp.indexOf(":,")+1,'0');
                    }
                    if(!tmp.isEmpty()){
                        while(tmp.indexOf('\r')!=-1)
                            tmp = tmp.replace("\r","");
                        while(tmp.indexOf('\n')!=-1)
                            tmp = tmp.replace("\n","");
                        if(toDataPool){
                            RowData_t data = extractRowData(tmp);
                            addToDataPool(data, enableSumCheck);
                        }
                        else{
                            packsBuff << tmp;
                        }
                    }
//                    qDebug()<<"match"<<match.captured(0);
                }
                else{
//                    qDebug()<<"no match";
                    if(index < inputArray.size()-1){
                        index++;
                        continue;
                    }else{
                        break;
                    }
                }
        } while(index < inputArray.length());

        restArray = inputArray.mid(index);

    }else if(protocolType == Float){
        QByteArray tmpArray = inputArray;
        while (tmpArray.indexOf(MAXDATA_AS_END)!=-1) {
            QByteArray before = tmpArray.mid(0,tmpArray.indexOf(MAXDATA_AS_END));
            tmpArray = tmpArray.mid(tmpArray.indexOf(MAXDATA_AS_END)+MAXDATA_AS_END.size());
            if(before.size()%4==0){
                if(toDataPool){
                    RowData_t data = extractRowData(before);
                    addToDataPool(data, enableSumCheck);
                }
                else{
                    packsBuff << before;
                }
            }
            else
                qDebug()<<"丢弃数据（长度不是4的倍数）："<<before.toHex().toUpper();
        }
        restArray = tmpArray;
    }
}

inline DataProtocol::Pack_t DataProtocol::popOnePack()
{
    Pack_t tmp;
    if(packsBuff.isEmpty()){
        return tmp;
    }
    tmp = packsBuff.at(0);
    packsBuff.pop_front();
    return tmp;
}

inline DataProtocol::RowData_t DataProtocol::extractRowData(const Pack_t &pack)
{

    RowData_t rowData;
    Pack_t dataPack;

    if(pack.isEmpty())
        return rowData;

    if(protocolType == Ascii){
        //把数据部分提取出来
        Pack_t dataPack = pack.mid(pack.indexOf(':'));

        QRegularExpression reg;
        QRegularExpressionMatch match;
        int index = 0;
        reg.setPattern("[\\+-]?\\d+\\.?\\d*");//匹配实数 符号出现0、1次，数字至少1次，小数点0、1次，小数不出现或出现多次
        do {
                match = reg.match(dataPack, index);
                if(match.hasMatch()) {
                    index = match.capturedEnd();
                    rowData << match.captured(0).toDouble();
    //                qDebug()<<match.captured(0).toDouble();
                }
                else{
    //                qDebug()<<"no match";
                    break;
                }
        } while(index < dataPack.length());

    }else if(protocolType == Float){
        dataPack = pack;
        while (dataPack.size()>0) {
            float tmp;
            if(packToFloat(dataPack, tmp)){
                dataPack = dataPack.mid(4);
                rowData << static_cast<double>(tmp);
//                qDebug("tmp:%f",tmp);
            }else{
//                qDebug()<<"get max";
            }
        }
    }

    return rowData;
}

inline void DataProtocol::addToDataPool(RowData_t &rowData, bool enableSumCheck=false)
{
    #define SUPER_MIN_VALUE 0.000000001
    if(rowData.size()<=0)
        return;

    if(enableSumCheck){
        //和校验模式 必须 要有至少两个数据
        if(rowData.size()<=1)
            return;
        double lastValue = rowData.at(rowData.size()-1);
        double sum = 0;
        for(int i = 0; i < rowData.size()-1; i++)
            sum += rowData.at(i);
        if(abs(sum-lastValue)>SUPER_MIN_VALUE){
            qDebug()<<rowData;
            return;
        }
        rowData.pop_back();
    }
    dataPool << rowData;
}

bool DataProtocol::byteArrayToFloat(const QByteArray& array, float& result)
{
    if(array.size()<4)
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
