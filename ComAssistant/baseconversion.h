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
extern QString hexFormat(QString const &data);
//显示模式转换
extern QString toHexDisplay(QString const &data);
extern QString toHexDisplay(bool needConvert, QString const &data);
extern QString toStringDisplay(QString &hexString);

//hex与bytearray互转
extern QString ByteArrayToHexString(QByteArray data);
extern QByteArray HexStringToByteArray(QString HexString);

#endif // BASECONVERSION_H
