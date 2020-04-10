#ifndef MYQCUSTOMPLOT_H
#define MYQCUSTOMPLOT_H

#include <QWidget>
#include <QObject>
#include <QInputDialog>
#include <QMessageBox>
#include <QtDebug>
#include <QString>

#include "qcustomplot.h"

#include "dataprotocol.h"
#include "qcustomplotcontrol.h"
#include "axistag.h"
#include "mytracer.h"

#include <QStatusBar>

class MyQCustomPlot:public QCustomPlot
{
    Q_OBJECT
public:
    MyQCustomPlot(QWidget* parent = nullptr);
    ~MyQCustomPlot();

    MyTracer *m_Tracer; //坐标跟随鼠标
    void init(QStatusBar* pBar, QCustomPlotControl* control, DataProtocol* proto);

private slots:
    void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
    void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
    void selectionChanged();
    void mousePress();
    void mouseWheel();
    void removeSelectedGraph();
    void removeAllGraphs();
    void hideSelectedGraph();
    void contextMenuRequest(QPoint pos);
    void moveLegend();
    void graphClicked(QCPAbstractPlottable *plottable, int dataIndex);
    void showTracer(QMouseEvent *event);

private:
    QStatusBar* bar;
    QCustomPlotControl* plotControl;
    DataProtocol* protocol;
};

#endif // MYQCUSTOMPLOT_H
