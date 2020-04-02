#ifndef BASECONVERSION_H
#define BASECONVERSION_H

#include <QString>
#include <QChar>
#include <QByteArray>
#include <QRegExp>
#include <QDebug>
#include <QMessageBox>

//十六进制格式检查
extern bool hexFormatCheck(QString const &data);
extern bool hexFormatCheck(char &data);
extern QString hexFormater(QString const &data); //在哪用了？
//显示模式转换
extern QString toHexDisplay(QByteArray const &data);
extern QString toHexDisplay(bool needConvert, QByteArray const &data);
extern QString toAsciiDisplay(QString &hexString, bool &isOK);

//hex与bytearray互转
extern QString ByteArrayToHexString(const QByteArray &data);
extern QByteArray HexStringToByteArray(QString HexString, bool &isOK);

#endif // BASECONVERSION_H
