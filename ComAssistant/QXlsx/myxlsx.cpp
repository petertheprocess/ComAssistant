#include "myxlsx.h"

MyXlsx::MyXlsx()
{

}

bool MyXlsx::write(QCustomPlot *customPlot, QString path)
{
    //xlsx对象
    Document xlsx;
    //列表头和值
    QString columTitle;
    double value;
    //写入坐标
    QString xlsxPos;
    int rowNumber = 1;
    char columnNumber = 'A';

    QSharedPointer<QCPGraphDataContainer> tmpContainer;
    int graphNum = customPlot->graphCount();

    //曲线编号遍历
    for(int j = 0; j < graphNum; j++){
        //曲线数据遍历
        tmpContainer = customPlot->graph(j)->data();
        for(int i = 0; i < tmpContainer->size(); ){
            if(rowNumber==1){
                //表头
                xlsxPos = QString(columnNumber) + QString::number(rowNumber);
                columTitle = customPlot->graph(j)->name();
                xlsx.write(xlsxPos, columTitle);
//                qDebug()<<xlsxPos<<columTitle;
            }else{
                xlsxPos = QString(columnNumber) + QString::number(rowNumber);
                value = (tmpContainer->constBegin()+i)->mainValue();
                xlsx.write(xlsxPos, value);
                i++; // rowNumber = 1时不应该占用一次循环
//                qDebug()<<xlsxPos<<value;
            }
            rowNumber++;
        }
        rowNumber = 1;
        columnNumber++;
        if(columnNumber>'Z'){
            qDebug()<<"graph number out of limit(24)";
            return false;
        }
    }

    if(!path.endsWith(".xlsx"))
        path.append(".xlsx");

    if (!xlsx.saveAs(path))
    {
        qDebug() << "Failed to save excel file";
        return false;
    }
    return true;
}
