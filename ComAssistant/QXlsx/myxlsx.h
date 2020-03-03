#ifndef MYXLSX_H
#define MYXLSX_H

#include "xlsxdocument.h"
#include "qcustomplot.h"
#include <QString>
#include <QSharedPointer>
#include <QtDebug>

using namespace QXlsx;

class MyXlsx
{
public:
    MyXlsx();
    static bool write(QCustomPlot *customPlot, QString path);
};

#endif // MYXLSX_H
