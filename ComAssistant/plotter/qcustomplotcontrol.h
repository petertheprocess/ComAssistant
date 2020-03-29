#ifndef QCUSTOMPLOTCONTROL_H
#define QCUSTOMPLOTCONTROL_H

#include "qcustomplot.h"
#include <QColor>
#include <QVector>
#include <QMenu>
#include <QInputDialog>
#include <algorithm>
#include "axistag.h"

using namespace std;

class QCustomPlotControl
{
public:
    typedef enum {
        Line,
        ScatterLine,
        Scatter
    }LineType_e;
    QCustomPlotControl();
    ~QCustomPlotControl();
    QCustomPlotControl(QCustomPlot* customPlot);

    bool addGraph(QCustomPlot* customPlot, int num=1);
    //清除指定曲线，-1清除所有曲线
    void clearPlotter(QCustomPlot* customPlot, int index);
    //调整x轴范围
    void adjustXRange(QCustomPlot* customPlot, const QCPRange& qcpRange);
    void adjustXRange(QCustomPlot* customPlot, bool enlarge);
    //把数据显示到绘图器上
    bool displayToPlotter(QCustomPlot* customPlot, const QVector<double>& rowData, bool refresh = true);
    //设置笔宽度
    void setupPenWidth(QCustomPlot* customPlot, double width = 1.5);
    //设置轴间距
    void setupAxisTick(QCustomPlot* customPlot, int axisTickLenth = 6, int axisSubTickLenth = 3);
    //设置轴标签
    void setupAxisLabel(QCustomPlot* customPlot, QString xLabel="Point number", QString yLabel="Value");
    //设置曲线名字
    bool setupPlotName(QCustomPlot* customPlot, QVector<QString> nameStr=QVector<QString>());
    //设置线型（线图、点线图、点图）: 点风格+线风格
    void setupLineType(QCustomPlot* customPlot, LineType_e type);
    //设置图例可见性
    void setupLegendVisible(QCustomPlot* customPlot, bool visible=true);
    //设置图像可见性
    void setupGraphVisible(QCustomPlot* customPlot, bool visible);
    //设置绘图器指针
    void setupCustomPlotPointer(QCustomPlot* pointer);
    //设置交互功能
    void setupInteractions(QCustomPlot* customPlot);
    //设置轴盒子
    void setupAxesBox(QCustomPlot* customPlot, bool connectRanges=false);
    //设置绘图器（总设置）
    void setupPlotter(QCustomPlot* customPlot);
    //读写名字集
    QVector<QString> getNameSet();
    void setNameSet(QCustomPlot* customPlot, QVector<QString> names);
    //最大图像数量
    int getMaxValidGraphNumber();
    //返回横坐标长度
    double getXAxisLength();
private:
    QCustomPlot* customPlot;
    QVector<QColor> colorSet;
    QVector<QString> nameSet;
    QCPRange xRange; //xRange的下限为xAxisCnt，上限为xRangeLengh
    int xRangeLengh = 200;
    const double zoomScale = 0.2;
    QVector<QCPScatterStyle::ScatterShape> scatterShapeSet;
//    AxisTag *mTag1;   //动态标签
    long int xAxisCnt = 0;
    LineType_e lineType = Line; //线型种类

    //设置点风格
    void setupScatterStyle(QCustomPlot* customPlot, bool enable=false);
    void setupScatterStyle(QCustomPlot* customPlot, QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssNone);
    //设置线风格
    void setupLineStyle(QCustomPlot* customPlot, QCPGraph::LineStyle style=QCPGraph::lsLine);
};

#endif // QCUSTOMPLOTCONTROL_H
