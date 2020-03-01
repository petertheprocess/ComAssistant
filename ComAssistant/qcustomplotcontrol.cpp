#include "qcustomplotcontrol.h"

QCustomPlotControl::QCustomPlotControl()
{
    //填充颜色，Candy色集
    colorSet << QColor(0xEF, 0x00, 0x00)
             << QColor(0x33, 0x66, 0x99)
             << QColor(0xFE, 0xC2, 0x11)
             << QColor(0x3B, 0xC3, 0x71)
             << QColor(0x66, 0x66, 0x99)
             << QColor(0x99, 0x99, 0x99)
             << QColor(0xFF, 0x66, 0x66)
             << QColor(0x66, 0x99, 0xCC)
             << QColor(0xCC, 0x66, 0x00)
             << QColor(0x00, 0x99, 0x99)
             << QColor(0x6B, 0x67, 0xBC)
             << QColor(0x99, 0x86, 0x7A)
             << QColor(0xCC, 0x33, 0x33)
             << QColor(0x66, 0x99, 0x99)
             << QColor(0xCC, 0x99, 0x00);

    scatterShapeSet << QCPScatterStyle::ssDot       ///< \enumimage{ssDot.png} a single pixel (use \ref ssDisc or \ref ssCircle if you want a round shape with a certain radius)
                    << QCPScatterStyle::ssCross     ///< \enumimage{ssCross.png} a cross
                    << QCPScatterStyle::ssPlus      ///< \enumimage{ssPlus.png} a plus
                    << QCPScatterStyle::ssCircle    ///< \enumimage{ssCircle.png} a circle
                    << QCPScatterStyle::ssDisc      ///< \enumimage{ssDisc.png} a circle which is filled with the pen's color (not the brush as with ssCircle)
                    << QCPScatterStyle::ssSquare    ///< \enumimage{ssSquare.png} a square
                    << QCPScatterStyle::ssDiamond   ///< \enumimage{ssDiamond.png} a diamond
                    << QCPScatterStyle::ssStar      ///< \enumimage{ssStar.png} a star with eight arms, i.e. a combination of cross and plus
                    << QCPScatterStyle::ssTriangle  ///< \enumimage{ssTriangle.png} an equilateral triangle, standing on baseline
                    << QCPScatterStyle::ssTriangleInverted ///< \enumimage{ssTriangleInverted.png} an equilateral triangle, standing on corner
                    << QCPScatterStyle::ssCrossSquare      ///< \enumimage{ssCrossSquare.png} a square with a cross inside
                    << QCPScatterStyle::ssPlusSquare       ///< \enumimage{ssPlusSquare.png} a square with a plus inside
                    << QCPScatterStyle::ssCrossCircle      ///< \enumimage{ssCrossCircle.png} a circle with a cross inside
                    << QCPScatterStyle::ssPlusCircle       ///< \enumimage{ssPlusCircle.png} a circle with a plus inside
                    << QCPScatterStyle::ssPeace     ///< \enumimage{ssPeace.png} a circle, with one vertical and two downward diagonal lines
                    << QCPScatterStyle::ssPixmap    ///< a custom pixmap specified by \ref setPixmap, centered on the data point coordinates
                    << QCPScatterStyle::ssCustom;    ///< custom painter operations are performed per scatter (As QPainterPath, see \ref setCustomPath)
}

QCustomPlotControl::QCustomPlotControl(QCustomPlot* customPlot)
{
    QCustomPlotControl();
    QCustomPlotControl::customPlot = customPlot;
    setupPlotter(customPlot);
}

QCustomPlotControl::~QCustomPlotControl()
{

}

bool QCustomPlotControl::addGraph(QCustomPlot* customPlot, int num)
{
    if(num<1 || num > colorSet.size())
        return false;

    if(customPlot->graphCount() >= colorSet.size())
        return false;

    int min = num > (colorSet.size() - customPlot->graphCount()) ? (colorSet.size() - customPlot->graphCount()) : num;

    for(int i = 0; i < min; i++){
        customPlot->addGraph();
    }

    setupPenWidth(customPlot);
    //设置点型
    setupScatterStyle(customPlot);
    //设置线型
    setupLineStyle(customPlot);

    customPlot->replot();

    return true;
}

/*
 * Function:把数据取出来显示到绘图器上
*/
bool QCustomPlotControl::displayToPlotter(QCustomPlot* customPlot, QVector<double> rowData)
{
    if(rowData.size() <= 0)
        return false;

    //判断是否需要添加曲线
    if(rowData.size()>customPlot->graphCount()){
        if(customPlot->graphCount()<colorSet.size()){
            addGraph(customPlot);
        }
    }

    //填充数据
    int minCnt = colorSet.size()>rowData.size()?rowData.size():colorSet.size();    
    for(int i = 0; i < minCnt; i++){
        customPlot->graph(i)->addData(xAxisCnt, rowData.at(i));
        //重算Y轴范围
        customPlot->graph(i)->rescaleValueAxis(false, true);
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    customPlot->xAxis->setRange(xAxisCnt, 200, Qt::AlignRight);
    customPlot->replot();
    xAxisCnt++;
    return true;
}

void QCustomPlotControl::clearPlotter(QCustomPlot* customPlot, int index)
{
    if(index > customPlot->graphCount()-1)
        return;

    QVector<double> empty;
    if(index == -1){
        for(int i = 0; i < customPlot->graphCount(); i++){
            customPlot->graph(i)->setData(empty, empty);
        }
    }else{
        customPlot->graph(index)->setData(empty, empty);
    }
    customPlot->replot();
}

void QCustomPlotControl::setupPenWidth(QCustomPlot* customPlot, double width)
{
    for(int i = 0; i < customPlot->graphCount(); i++){
        //设置画笔
        QPen pen;
        pen.setColor(colorSet.at(i));
        pen.setWidthF(width);
        customPlot->graph(i)->setPen(pen);
    }
    customPlot->replot();
}

void QCustomPlotControl::setupAxisTick(QCustomPlot* customPlot, int axisTickLenth, int axisSubTickLenth)
{
    //4边的轴
    customPlot->axisRect()->setupFullAxesBox();

    customPlot->xAxis->setTickLengthIn(axisTickLenth);
    customPlot->yAxis->setTickLengthIn(axisTickLenth);
    customPlot->xAxis->setSubTickLengthIn(axisSubTickLenth);
    customPlot->yAxis->setSubTickLengthIn(axisSubTickLenth);

    customPlot->xAxis2->setTickLengthIn(axisTickLenth);
    customPlot->xAxis2->setSubTickLengthIn(axisSubTickLenth);
    customPlot->yAxis2->setTickLengthIn(axisTickLenth);
    customPlot->yAxis2->setSubTickLengthIn(axisSubTickLenth);
}

void QCustomPlotControl::setupAxisLabel(QCustomPlot* customPlot, QString xLabel, QString yLabel)
{
    customPlot->xAxis->setLabel(xLabel);
    customPlot->yAxis->setLabel(yLabel);
}

bool QCustomPlotControl::setupPlotName(QCustomPlot* customPlot, QVector<QString> nameStr)
{
    if(nameStr.size() <= 0)
        return false;
    QString name;
    int size;
    size = nameStr.size()>customPlot->graphCount() ? customPlot->graphCount() : nameStr.size();
    for(int i = 0; i < size; i++){
        customPlot->graph(i)->setName(nameStr.at(0));
        nameStr.pop_front();
    }
    return true;
}

void QCustomPlotControl::setupScatterStyle(QCustomPlot* customPlot, QCPScatterStyle::ScatterShape shape)
{
    if(shape != QCPScatterStyle::ssNone){
        for(int i = 0; i < customPlot->graphCount(); i++){
            customPlot->graph(i)->setScatterStyle(QCPScatterStyle(shape,2));
        }
    }else{
        for(int i = 0; i < customPlot->graphCount(); i++){
            customPlot->graph(i)->setScatterStyle(shape);
        }
    }
    customPlot->replot();
}

void QCustomPlotControl::setupLineStyle(QCustomPlot* customPlot, QCPGraph::LineStyle style)
{
    for(int i = 0; i < customPlot->graphCount(); i++){
        customPlot->graph(i)->setLineStyle(style);
    }
    customPlot->replot();
}

void QCustomPlotControl::setupLegendVisible(QCustomPlot* customPlot, bool visible)
{
    if(visible){
        customPlot->legend->setVisible(true);
    }else{
        customPlot->legend->setVisible(false);
    }
}

void QCustomPlotControl::setupGraphVisible(QCustomPlot* customPlot, bool visible)
{
    if(visible){
        customPlot->legend->setVisible(true);
    }else{
        customPlot->legend->setVisible(false);
    }
}

void QCustomPlotControl::setupCustomPlotPointer(QCustomPlot* pointer)
{
    customPlot=pointer;
}

void QCustomPlotControl::setupInteractions(QCustomPlot* customPlot)
{
    //设置交互功能：范围可拖拽、范围可缩放、轴可选择、图例可选择、绘图区可选择
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                    QCP::iSelectLegend | QCP::iSelectPlottables);
    //设置仅图例框中的条目可被选择
    customPlot->legend->setSelectableParts(QCPLegend::spItems);

    //与槽相关的许多函数放在了MainWindow里
    //...
}

void QCustomPlotControl::setupPlotter(QCustomPlot* customPlot)
{
    //设置一个指针直接控制customPlot解决槽函数要访问ui类下的customPlot的问题
    QCustomPlotControl::customPlot = customPlot;

    //添加一条示例曲线。更多曲线的添加动态完成。
    if(customPlot->graphCount()==0){
        customPlot->addGraph();
    }

    //设置自适应采样，提高大数据性能
    for(int i = 0; i < customPlot->graphCount(); i++){
        customPlot->graph(0)->setAdaptiveSampling(true);
    }
    //设置画笔
    setupPenWidth(customPlot);
    //设置轴间隔
    setupAxisTick(customPlot);
    //设置轴标签
    setupAxisLabel(customPlot);
    //设置曲线名称
    setupPlotName(customPlot);
    //设置点型
    setupScatterStyle(customPlot);
    //设置线型
    setupLineStyle(customPlot);
    //设置标签
    setupLegendVisible(customPlot, true);
    //设置标签位置
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignLeft|Qt::AlignTop);
    //设置交互功能
    setupInteractions(customPlot);

    // make left and bottom axes transfer their ranges to right and top axes:
    QObject::connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    QObject::connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

}
