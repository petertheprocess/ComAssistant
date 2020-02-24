#include "baseconversion.h"

/*
 * Function:检查传入的数据是否满足十六进制格式
 * Note:只允许' ', 'x', 'X', 'a'~'f', 'A'~'F', '0'~'9'
*/
bool hexFormatCheck(QString const &data)
{
    int cnt = 0;

    //空数据不做处理
    if(data.size()==0)
        return true;

    //大写化
    QString tmp = data.toUpper();

    //剔除0x前缀
    while(tmp.indexOf("0X") != -1)
        tmp.remove("0X");

    //避免“x2 2x x3”这类数据格式
    if(tmp.indexOf("X ")!=-1 || tmp.indexOf(" X")!=-1 ||
       tmp.indexOf("X")==0 || tmp.indexOf("X")==(tmp.size()-1))
        return false;

    //格式检查
    for(int i = 0; i < tmp.size(); i++){
        if((tmp[i]>='0' && tmp[i]<='9') ||
           (tmp[i]>='A' && tmp[i]<='F') ||
            tmp[i]=='X' || tmp[i]==' '){

            //最大FF，不允许连续三个合法字符
            if(tmp[i] != ' ')
                cnt++;
            else
                cnt = 0;
            if(cnt>2)
                return false;

            continue;
        }
        else {
            return false;
        }
    }
    return true;
}

/*
 * Function:传入数据转换为十六进制格式
 * Output:每个数之间用空格表示
*/
QString hexFormat(QString const &data)
{
    int cnt = 0;

    //空数据不做处理
    if(data.size()==0)
        return "";

    //大写化
    QString tmp = data.toUpper();

    //剔除0x前缀
    while(tmp.indexOf("0X") != -1)
        tmp.remove("0X");

    //避免“x2 2x x3”这类数据格式
    if(tmp.indexOf("X ")!=-1 || tmp.indexOf(" X")!=-1 ||
       tmp.indexOf("X")==0 || tmp.indexOf("X")==(tmp.size()-1))
        return "";

    //格式检查
    for(int i = 0; i < tmp.size(); i++){
        if((tmp[i]>='0' && tmp[i]<='9') ||
           (tmp[i]>='A' && tmp[i]<='F') ||
            tmp[i]=='X' || tmp[i]==' '){

            //最大FF，不允许连续三个合法字符
            if(tmp[i] != ' ')
                cnt++;
            else
                cnt = 0;
            if(cnt>2)
                return "";

            continue;
        }
        else {
            return "";
        }
    }

    //删除多余空格
    tmp.replace(QRegExp("\\s{1,}"), " ");

    return tmp;
}

/*
 * Function:检查传入的数据是否满足十六进制格式
*/
bool hexFormatCheck(char &data)
{
    if((data>='0' && data<='9') ||
       (data>='a' && data<='f') ||
       (data>='A' && data<='F') ||
        data=='x' || data<='X'  || data<=' '){
        return true;
    }
    else {
        return false;
    }
}

QString ByteArrayToHexString(QByteArray data)
{
    QString ret(data.toHex().toUpper());//转为16进制大写
    int originLen = ret.length()/2;
    for(int i = 1; i < originLen; i++)
    {
       ret.insert(2*i+i-1," ");//编写格式
    }
    ret.append(' ');
    return ret;
}

QByteArray HexStringToByteArray(QString HexString)
{
    bool ok;
    QByteArray ret;
    HexString = HexString.trimmed();
    HexString = HexString.simplified();
    QStringList sl = HexString.split(" ");

    foreach (QString s, sl) {
        if(!s.isEmpty())
        {
            char c = static_cast<char>(s.toInt(&ok,16)&0xFF);
            if(ok){
                ret.append(c);
            }else{
//                qDebug()<<"非法的16进制字符："<<s;
                QMessageBox::warning(nullptr,"警告",QString("非法的16进制字符: \"%1\"").arg(s));
            }
        }
    }
    return ret;
}

/*
 * Function:将数据转换为十六进制显示
*/
QString toHexDisplay(QString const &data)
{
    QString res;
    res = ByteArrayToHexString(data.toUtf8());
    return res;
}

/*
 * Function:带开关的十六进制转换
*/
QString toHexDisplay(bool needConvert, QString const &data)
{
    if(needConvert)
        return toHexDisplay(data);
    else
        return data;
}

QString toStringDisplay(QString &hexString)
{
    return HexStringToByteArray(hexString);
}
