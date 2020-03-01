#ifndef QCUSTOMPLOTCONTROL_H
#define QCUSTOMPLOTCONTROL_H

#include "qcustomplot.h"
#include <QColor>
#include <QVector>
#include <QMenu>
#include <QInputDialog>

class QCustomPlotControl
{
public:
    QCustomPlotControl();
    ~QCustomPlotControl();
    QCustomPlotControl(QCustomPlot* customPlot);

    bool addGraph(QCustomPlot* customPlot, int num=1);
    //清除指定曲线，-1清除所有曲线
    void clearPlotter(QCustomPlot* customPlot, int index);
    //把数据显示到绘图器上
    bool displayToPlotter(QCustomPlot* customPlot, QVector<double> rowData);
    //设置笔宽度
    void setupPenWidth(QCustomPlot* customPlot, double width = 1.5);
    //设置轴间距
    void setupAxisTick(QCustomPlot* customPlot, int axisTickLenth = 6, int axisSubTickLenth = 3);
    //设置轴标签
    void setupAxisLabel(QCustomPlot* customPlot, QString xLabel="Point number", QString yLabel="Value");
    //设置曲线名字
    bool setupPlotName(QCustomPlot* customPlot, QVector<QString> nameStr=QVector<QString>());
    //设置点型
    void setupScatterStyle(QCustomPlot* customPlot, QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssNone);
    //设置线型
    void setupLineStyle(QCustomPlot* customPlot, QCPGraph::LineStyle style=QCPGraph::lsLine);
    //设置图例可见性
    void setupLegendVisible(QCustomPlot* customPlot, bool visible=true);
    //设置图像可见性
    void setupGraphVisible(QCustomPlot* customPlot, bool visible);
    //设置绘图器指针
    void setupCustomPlotPointer(QCustomPlot* pointer);
    //设置交互功能
    void setupInteractions(QCustomPlot* customPlot);
    //设置绘图器（总设置）
    void setupPlotter(QCustomPlot* customPlot);

private:
    QCustomPlot* customPlot;
    QVector<QColor> colorSet;
    QVector<QCPScatterStyle::ScatterShape> scatterShapeSet;
    unsigned int xAxisCnt = 0;
//    int currentGraphNumber = 0;
};

#endif // QCUSTOMPLOTCONTROL_H
