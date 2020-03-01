#ifndef QCUSTOMPLOTCONTROL_H
#define QCUSTOMPLOTCONTROL_H

#include "qcustomplot.h"
#include <QColor>
#include <QVector>

class QCustomPlotControl
{
public:
    QCustomPlotControl();
    ~QCustomPlotControl();
    QCustomPlotControl(QCustomPlot* customPlot);
    void clearPlotter(QCustomPlot* customPlot, int index);
    bool displayToPlotter(QCustomPlot* customPlot, QVector<double> rowData);
    void setupPenWidth(QCustomPlot* customPlot, double width = 1.5);
    void setupAxisTick(QCustomPlot* customPlot, int axisTickLenth = 6, int axisSubTickLenth = 3);
    void setupAxisLabel(QCustomPlot* customPlot, QString xLabel="Point number", QString yLabel="Value");
    bool setupPlotName(QCustomPlot* customPlot, QVector<QString> nameStr=QVector<QString>());
    void setupScatterStyle(QCustomPlot* customPlot, QCPScatterStyle::ScatterShape shape=QCPScatterStyle::ssNone);
    void setupLineStyle(QCustomPlot* customPlot, QCPGraph::LineStyle style=QCPGraph::lsLine);
    void setupLegendVisible(QCustomPlot* customPlot, bool visible=true);
    void setupGraphVisible(QCustomPlot* customPlot, bool visible);
    void setupPlotter(QCustomPlot* customPlot);

private:
    QVector<QColor> colorSet;
    QVector<QCPScatterStyle::ScatterShape> scatterShapeSet;
    unsigned int xAxisCnt = 0;
};

#endif // QCUSTOMPLOTCONTROL_H
