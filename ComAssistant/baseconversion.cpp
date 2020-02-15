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

/*
 * Function:将数据转换为十六进制显示
*/
QString toHexDisplay(QString const &data)
{
    QString res, tmp;
    bool ok;
    qDebug()<<"t"<<*(data.data()+3);
    for (int i = 0; i < data.size(); i++) {
        tmp = data[i];
        res = res + QString::number(tmp.toLatin1().at(0)).toUpper()+" ";
    }
//    qDebug()<<res<<endl;
    return res;
}
