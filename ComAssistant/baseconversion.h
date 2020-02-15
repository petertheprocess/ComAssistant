#ifndef BASECONVERSION_H
#define BASECONVERSION_H

#include <QString>
#include <QChar>
#include <QByteArray>
#include <QRegExp>
#include <QDebug>

extern bool hexFormatCheck(QString const &data);
extern bool hexFormatCheck(char &data);
extern QString hexFormat(QString const &data);
extern QString toHexDisplay(QString const &data);

#endif // BASECONVERSION_H
